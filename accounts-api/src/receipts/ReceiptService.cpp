#include "receipts/ReceiptService.hpp"

#include "receipts/ImageFormat.hpp"
#include "util/Crypto.hpp"

#include <algorithm>
#include <cctype>

namespace accounts::receipts {

using boost::asio::awaitable;

namespace {

constexpr int defaultPageSize = 50;
constexpr int maxPageSize = 100;

std::unexpected<AppError> receiptNotFound() {
    return fail(ErrorCode::NotFound, "receipt not found");
}

// Turns "no such row" into a NotFound error, so callers get a plain Result<Receipt>.
Result<Receipt> requireFound(Result<std::optional<Receipt>> found) {
    if (!found) {
        return std::unexpected(found.error());
    }
    if (!*found) {
        return receiptNotFound();
    }
    return std::move(**found);
}

std::string toLower(std::string text) {
    std::ranges::transform(text, text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text;
}

// One object per receipt; uploading a new photo overwrites the old one.
std::string imageObjectKey(const std::string &userId, const std::string &receiptId) {
    return "receipts/" + userId + "/" + receiptId;
}

} // namespace

ReceiptService::ReceiptService(ReceiptRepository &repository, storage::ObjectStore &objectStore)
    : repository_(repository), objectStore_(objectStore) {}

awaitable<Result<Receipt>> ReceiptService::create(const std::string &userId, ReceiptDraft draft) {
    if (auto valid = validate(draft); !valid) {
        co_return std::unexpected(valid.error());
    }
    co_return co_await repository_.create(userId, draft);
}

awaitable<Result<Receipt>> ReceiptService::get(const std::string &userId, const std::string &id) {
    if (!isUuid(id)) {
        co_return receiptNotFound();
    }
    co_return requireFound(co_await repository_.find(userId, id));
}

awaitable<Result<ReceiptPage>> ReceiptService::list(const std::string &userId, std::optional<std::string> cursor,
                                                    std::optional<int> limit) {
    const int pageSize = std::clamp(limit.value_or(defaultPageSize), 1, maxPageSize);

    std::optional<PageCursor> after;
    if (cursor) {
        after = decodeCursor(*cursor);
        if (!after) {
            co_return fail(ErrorCode::InvalidInput, "cursor is invalid");
        }
    }

    // Ask for one extra row: if it exists, there is a next page.
    auto receipts = co_await repository_.list(userId, after, pageSize + 1);
    if (!receipts) {
        co_return std::unexpected(receipts.error());
    }
    ReceiptPage page{std::move(*receipts), std::nullopt};
    if (page.items.size() > static_cast<std::size_t>(pageSize)) {
        page.items.pop_back();
        const Receipt &last = page.items.back();
        page.nextCursor = encodeCursor({last.purchasedAt, last.id});
    }
    co_return page;
}

awaitable<Result<Receipt>> ReceiptService::update(const std::string &userId, const std::string &id,
                                                  ReceiptChanges changes) {
    if (!isUuid(id)) {
        co_return receiptNotFound();
    }
    if (auto valid = validate(changes); !valid) {
        co_return std::unexpected(valid.error());
    }
    co_return requireFound(co_await repository_.update(userId, id, changes));
}

awaitable<Result<void>> ReceiptService::remove(const std::string &userId, const std::string &id) {
    if (!isUuid(id)) {
        co_return receiptNotFound();
    }
    auto removed = co_await repository_.remove(userId, id);
    if (!removed) {
        co_return std::unexpected(removed.error());
    }
    if (!*removed) {
        co_return receiptNotFound();
    }
    co_return Result<void>{};
}

awaitable<Result<Receipt>> ReceiptService::uploadImage(const std::string &userId, const std::string &id,
                                                       ImageUpload upload) {
    // Check ownership first, so nobody can write objects under another user's prefix.
    auto receipt = co_await get(userId, id);
    if (!receipt) {
        co_return std::unexpected(receipt.error());
    }

    const auto mimeType = detectImageMimeType(upload.bytes);
    if (!mimeType) {
        co_return fail(ErrorCode::InvalidInput, "image must be JPEG, PNG or WebP");
    }
    const std::string sha256 = crypto::sha256Hex(upload.bytes);
    if (!crypto::constantTimeEquals(sha256, toLower(upload.declaredSha256))) {
        co_return fail(ErrorCode::InvalidInput, "X-Content-SHA256 does not match the uploaded bytes");
    }

    const ReceiptImage image{
        .objectKey = imageObjectKey(userId, id),
        .sha256 = sha256,
        .sizeBytes = static_cast<std::int64_t>(upload.bytes.size()),
        .mimeType = std::string(*mimeType),
    };
    // Upload before recording: a crash in between leaves an unreferenced
    // object (harmless), never a row pointing at a missing one.
    if (auto stored = co_await objectStore_.put(image.objectKey, std::move(upload.bytes), image.mimeType); !stored) {
        co_return std::unexpected(stored.error());
    }
    co_return requireFound(co_await repository_.attachImage(userId, id, image));
}

awaitable<Result<storage::StoredObject>> ReceiptService::downloadImage(const std::string &userId,
                                                                       const std::string &id) {
    auto receipt = co_await get(userId, id);
    if (!receipt) {
        co_return std::unexpected(receipt.error());
    }
    if (!receipt->image) {
        co_return fail(ErrorCode::NotFound, "receipt has no image");
    }
    auto object = co_await objectStore_.get(receipt->image->objectKey);
    if (!object) {
        co_return std::unexpected(object.error());
    }
    if (!*object) {
        co_return fail(ErrorCode::NotFound, "image not found");
    }
    co_return std::move(**object);
}

} // namespace accounts::receipts
