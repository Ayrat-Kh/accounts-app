#include "app/SyncEngine.hpp"
#include "support/Fakes.hpp"
#include "support/TempDirectory.hpp"
#include "util/Files.hpp"

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include <format>
#include <map>
#include <set>

using namespace receipts;
using namespace receipts::testing;

namespace {

// A tiny in-memory accounts-api: enough of it for the sync protocol.
class FakeServer {
public:
    std::map<std::string, std::int64_t> receipts; // id -> amount
    std::set<std::string> withImage;
    bool offline = false;
    int nextId = 1;

    Result<net::HttpResponse> handle(const net::HttpRequest &request) {
        if (offline) {
            return fail(ErrorCode::Network, "no route to host");
        }
        const std::string path = request.url.substr(std::string("https://api.test").size());
        if (request.method == "POST" && path == "/v1/receipts") {
            const auto body = nlohmann::json::parse(request.body);
            const std::string id = "r-" + std::to_string(nextId++);
            receipts[id] = body["amount_minor"].get<std::int64_t>();
            return jsonResponse(201, receiptJson(id));
        }
        if (request.method == "PUT" && path.ends_with("/image")) {
            withImage.insert(path.substr(13, path.size() - 13 - 6));
            return jsonResponse(200, "{}");
        }
        if (request.method == "DELETE") {
            receipts.erase(path.substr(13));
            return net::HttpResponse{204, {}, ""};
        }
        if (request.method == "GET" && path.starts_with("/v1/receipts")) {
            std::string items;
            for (const auto &[id, amount] : receipts) {
                items += (items.empty() ? "" : ",") + receiptJson(id);
            }
            return jsonResponse(200, "{\"items\": [" + items + "], \"next_cursor\": null}");
        }
        return jsonResponse(404, "{}");
    }

private:
    std::string receiptJson(const std::string &id) {
        return std::format(R"({{"id": "{}", "amount_minor": {}, "currency": "EUR",
            "purchased_at": "2026-10-13T12:00:00.000Z", "merchant": null, "ocr_confidence": null,
            "image": {}}})",
                           id, receipts[id], withImage.contains(id) ? R"({"sha256": "x"})" : "null");
    }
};

struct Fixture {
    FakeServer server;
    std::shared_ptr<FakeHttpTransport> transport =
        std::make_shared<FakeHttpTransport>([this](const net::HttpRequest &request) { return server.handle(request); });
    std::shared_ptr<MemorySecureStore> secrets = [] {
        auto store = std::make_shared<MemorySecureStore>();
        store->write("accounts.access_token", "token");
        store->write("accounts.refresh_token", "refresh");
        return store;
    }();
    api::ApiClient api{transport, "https://api.test", secrets};
    TempDirectory directory;
    std::unique_ptr<store::LocalReceiptStore> store =
        store::LocalReceiptStore::open(directory.file("receipts.db")).value();
    RecordingLogger logger;
    app::SyncEngine sync{api, *store, logger};

    void saveLocally(const std::string &localId, std::int64_t amount, std::optional<std::string> image = {}) {
        REQUIRE(
            store->addPending({.money = {amount, "EUR"}, .purchasedAt = "2026-10-13T12:00:00.000Z"}, localId, image));
    }
};

} // namespace

TEST_CASE("pending receipts and their photos are uploaded, then marked synced") {
    Fixture fixture;
    const std::string photo = fixture.directory.file("photo.jpg");
    REQUIRE(files::writeAtomically(photo, "\xFF\xD8\xFF jpeg"));
    fixture.saveLocally("local-1", 1234, photo);

    const auto report = fixture.sync.sync().value();

    CHECK(report.uploaded == 1);
    CHECK(fixture.server.receipts.at("r-1") == 1234);
    CHECK(fixture.server.withImage.contains("r-1"));
    const auto local = fixture.store->find("local-1").value();
    CHECK(local->syncState == SyncState::Synced);
    CHECK(local->serverId == "r-1");
}

TEST_CASE("offline: nothing is lost and the next sync finishes the job") {
    Fixture fixture;
    fixture.saveLocally("local-1", 500);
    fixture.server.offline = true;

    const auto failed = fixture.sync.sync();
    REQUIRE_FALSE(failed);
    CHECK(failed.error().code == ErrorCode::Network);
    CHECK(fixture.store->find("local-1").value()->syncState == SyncState::PendingUpload);

    fixture.server.offline = false;
    CHECK(fixture.sync.sync().value().uploaded == 1);
    CHECK(fixture.server.receipts.size() == 1); // no duplicate
}

TEST_CASE("deletes are pushed, and receipts from other devices are pulled") {
    Fixture fixture;
    fixture.saveLocally("local-1", 100);
    REQUIRE(fixture.sync.sync());
    fixture.server.receipts["r-99"] = 4200; // created on another phone

    REQUIRE(fixture.store->markPendingDelete("local-1"));
    const auto report = fixture.sync.sync().value();

    CHECK(report.deleted == 1);
    CHECK(report.downloaded == 1);
    CHECK_FALSE(fixture.server.receipts.contains("r-1"));
    const auto visible = fixture.store->listVisible().value();
    REQUIRE(visible.size() == 1);
    CHECK(visible[0].money.amountMinor == 4200);
}
