#include "models/ModelDownloader.hpp"
#include "support/Fakes.hpp"
#include "support/TempDirectory.hpp"
#include "util/Files.hpp"

#include <catch2/catch_test_macros.hpp>

#include <format>
#include <regex>

using namespace receipts;
using namespace receipts::testing;

namespace {

// Serves a manifest and range requests over two in-memory "files".
class FakeAssetServer {
public:
    std::map<std::string, std::string> files = {
        {"eng.traineddata", std::string(10'000'000, 'e')}, // > 8 MiB: needs two pieces
        {"model.gguf", "tiny model"},
    };
    int pieceRequests = 0;
    bool corrupt = false;

    Result<net::HttpResponse> handle(const net::HttpRequest &request) {
        if (request.url.ends_with("/v1/assets")) {
            std::string manifest = "[";
            for (const auto &[name, bytes] : files) {
                manifest += std::format(R"({{"name": "{}", "size": {}, "sha256": "{}"}},)", name, bytes.size(),
                                        files::sha256Hex(bytes));
            }
            manifest.back() = ']';
            return jsonResponse(200, manifest);
        }
        ++pieceRequests;
        const std::string name = request.url.substr(request.url.rfind('/') + 1);
        std::smatch range;
        const std::string header = request.headers.at("Range");
        std::regex_match(header, range, std::regex(R"(bytes=(\d+)-(\d+))"));
        const auto first = std::stoull(range[1]);
        const auto last = std::stoull(range[2]);
        std::string piece = files.at(name).substr(first, last - first + 1);
        if (corrupt) {
            piece[0] ^= 1;
        }
        return net::HttpResponse{
            206, {{"content-range", std::format("bytes {}-{}/{}", first, last, files.at(name).size())}}, piece};
    }
};

struct Fixture {
    FakeAssetServer server;
    std::shared_ptr<FakeHttpTransport> transport =
        std::make_shared<FakeHttpTransport>([this](const net::HttpRequest &request) { return server.handle(request); });
    std::shared_ptr<MemorySecureStore> secrets = [] {
        auto store = std::make_shared<MemorySecureStore>();
        store->write("accounts.access_token", "token");
        return store;
    }();
    api::ApiClient api{transport, "https://api.test", secrets};
    RecordingLogger logger;
    TempDirectory directory;
    models::ModelDownloader downloader{api, directory.path(), logger};
};

} // namespace

TEST_CASE("all assets download in pieces and land in the right place") {
    Fixture fixture;
    REQUIRE(fixture.downloader.downloadAll(nullptr));

    CHECK(fixture.downloader.hasAll({"eng.traineddata", "model.gguf"}));
    CHECK(std::filesystem::exists(fixture.directory.path() / "tessdata" / "eng.traineddata"));
    CHECK(files::readAll(fixture.directory.path() / "model.gguf").value() == "tiny model");
    CHECK(fixture.server.pieceRequests == 3); // two for the 10 MB file, one for the small one
}

TEST_CASE("an interrupted download resumes where it stopped") {
    Fixture fixture;
    std::filesystem::create_directories(fixture.directory.path() / "tessdata");
    // 8 MiB already on disk from an earlier attempt.
    REQUIRE(files::append(fixture.directory.path() / "tessdata" / "eng.traineddata.part",
                          fixture.server.files["eng.traineddata"].substr(0, 8 * 1024 * 1024)));

    REQUIRE(fixture.downloader.downloadAll(nullptr));
    CHECK(fixture.server.pieceRequests == 2); // only the rest of the big file, plus the small one
}

TEST_CASE("a corrupted download is discarded, never installed") {
    Fixture fixture;
    fixture.server.corrupt = true;

    const auto result = fixture.downloader.downloadAll(nullptr);
    REQUIRE_FALSE(result);
    CHECK_FALSE(fixture.downloader.hasAll({"eng.traineddata"}));
    CHECK_FALSE(std::filesystem::exists(fixture.directory.path() / "tessdata" / "eng.traineddata.part"));
}

TEST_CASE("asset names that could escape the models directory are refused") {
    Fixture fixture;
    fixture.server.files = {{"../../evil", "x"}};
    CHECK_FALSE(fixture.downloader.downloadAll(nullptr));
}
