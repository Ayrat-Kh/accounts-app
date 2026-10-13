#include "receipts/ReceiptRoutes.hpp"

#include "receipts/ReceiptJson.hpp"

#include <charconv>

namespace accounts::receipts {

using boost::asio::awaitable;
using http::Method;
using http::Request;
using http::Response;
using http::Status;

namespace {

const auto receiptToJson = [](const Receipt &receipt) { return toJson(receipt); };
const auto pageToJson = [](const ReceiptPage &page) { return toJson(page); };

Result<std::optional<int>> parseLimit(const Request &request) {
    const auto text = request.queryParam("limit");
    if (!text) {
        return std::nullopt;
    }
    int limit = 0;
    const auto [end, error] = std::from_chars(text->data(), text->data() + text->size(), limit);
    if (error != std::errc{} || end != text->data() + text->size()) {
        return fail(ErrorCode::InvalidInput, "limit must be an integer");
    }
    return limit;
}

awaitable<Response> createReceipt(ReceiptService &service, Request &request) {
    auto body = http::parseJsonObject(request.body);
    if (!body) {
        co_return http::errorResponse(body.error());
    }
    auto draft = draftFromJson(*body);
    if (!draft) {
        co_return http::errorResponse(draft.error());
    }
    co_return http::respond(co_await service.create(request.authenticatedUser().userId, std::move(*draft)),
                            Status::Created, receiptToJson);
}

awaitable<Response> listReceipts(ReceiptService &service, Request &request) {
    auto limit = parseLimit(request);
    if (!limit) {
        co_return http::errorResponse(limit.error());
    }
    co_return http::respond(
        co_await service.list(request.authenticatedUser().userId, request.queryParam("cursor"), *limit), Status::Ok,
        pageToJson);
}

awaitable<Response> getReceipt(ReceiptService &service, Request &request) {
    co_return http::respond(
        co_await service.get(request.authenticatedUser().userId, std::string(request.pathParam("id"))), Status::Ok,
        receiptToJson);
}

awaitable<Response> updateReceipt(ReceiptService &service, Request &request) {
    auto body = http::parseJsonObject(request.body);
    if (!body) {
        co_return http::errorResponse(body.error());
    }
    auto changes = changesFromJson(*body);
    if (!changes) {
        co_return http::errorResponse(changes.error());
    }
    co_return http::respond(co_await service.update(request.authenticatedUser().userId,
                                                    std::string(request.pathParam("id")), std::move(*changes)),
                            Status::Ok, receiptToJson);
}

awaitable<Response> deleteReceipt(ReceiptService &service, Request &request) {
    auto removed = co_await service.remove(request.authenticatedUser().userId, std::string(request.pathParam("id")));
    co_return removed ? http::noContentResponse() : http::errorResponse(removed.error());
}

awaitable<Response> uploadImage(ReceiptService &service, Request &request) {
    ImageUpload upload{std::move(request.body), std::string(request.header("x-content-sha256"))};
    co_return http::respond(co_await service.uploadImage(request.authenticatedUser().userId,
                                                         std::string(request.pathParam("id")), std::move(upload)),
                            Status::Ok, receiptToJson);
}

awaitable<Response> downloadImage(ReceiptService &service, Request &request) {
    auto image =
        co_await service.downloadImage(request.authenticatedUser().userId, std::string(request.pathParam("id")));
    if (!image) {
        co_return http::errorResponse(image.error());
    }
    co_return http::bytesResponse(Status::Ok, std::move(image->bytes), image->contentType);
}

} // namespace

void registerReceiptRoutes(http::Router &router, ReceiptService &service, std::size_t maxImageBytes) {
    // Each handler is a free function above; the lambdas only bind the service.
    auto bind = [&service](auto handler) {
        return [&service, handler](Request &request) { return handler(service, request); };
    };

    router.add(Method::Post, "/v1/receipts", bind(createReceipt));
    router.add(Method::Get, "/v1/receipts", bind(listReceipts));
    router.add(Method::Get, "/v1/receipts/:id", bind(getReceipt));
    router.add(Method::Patch, "/v1/receipts/:id", bind(updateReceipt));
    router.add(Method::Delete, "/v1/receipts/:id", bind(deleteReceipt));
    router.add(Method::Put, "/v1/receipts/:id/image", bind(uploadImage),
               {.requiresAuth = true, .maxBodyBytes = maxImageBytes});
    router.add(Method::Get, "/v1/receipts/:id/image", bind(downloadImage));
}

} // namespace accounts::receipts
