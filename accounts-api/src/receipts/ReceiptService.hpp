#pragma once

// Receipt use cases: validation, "not found" semantics, pagination and the
// image workflow. The routes only translate HTTP <-> these calls.

#include "receipts/ReceiptRepository.hpp"
#include "storage/ObjectStore.hpp"

namespace accounts::receipts {

struct ImageUpload {
    std::string bytes;
    std::string declaredSha256; // from the X-Content-SHA256 header; proves the upload arrived intact
};

class ReceiptService {
public:
    ReceiptService(ReceiptRepository &repository, storage::ObjectStore &objectStore);

    boost::asio::awaitable<Result<Receipt>> create(const std::string &userId, ReceiptDraft draft);
    boost::asio::awaitable<Result<Receipt>> get(const std::string &userId, const std::string &id);
    boost::asio::awaitable<Result<ReceiptPage>> list(const std::string &userId, std::optional<std::string> cursor,
                                                     std::optional<int> limit);
    boost::asio::awaitable<Result<Receipt>> update(const std::string &userId, const std::string &id,
                                                   ReceiptChanges changes);
    boost::asio::awaitable<Result<void>> remove(const std::string &userId, const std::string &id);

    boost::asio::awaitable<Result<Receipt>> uploadImage(const std::string &userId, const std::string &id,
                                                        ImageUpload upload);
    boost::asio::awaitable<Result<storage::StoredObject>> downloadImage(const std::string &userId,
                                                                        const std::string &id);

private:
    ReceiptRepository &repository_;
    storage::ObjectStore &objectStore_;
};

} // namespace accounts::receipts
