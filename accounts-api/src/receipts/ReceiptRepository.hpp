#pragma once

// All SQL for receipts. Every method that touches a single receipt takes the
// owner's user id and filters on it, so one user can never read or change
// another user's receipt (the IDOR bug in the old PUT /v1/accounts/:id).
// A receipt owned by someone else therefore looks exactly like a missing one.

#include "db/PgPool.hpp"
#include "receipts/Receipt.hpp"

#include <boost/asio/awaitable.hpp>

namespace accounts::receipts {

class ReceiptRepository {
public:
    explicit ReceiptRepository(db::PgPool &pool) : pool_(pool) {}

    boost::asio::awaitable<Result<Receipt>> create(const std::string &userId, const ReceiptDraft &draft);

    boost::asio::awaitable<Result<std::optional<Receipt>>> find(const std::string &userId, const std::string &id);

    // Newest purchase first, starting strictly after `after`.
    boost::asio::awaitable<Result<std::vector<Receipt>>> list(const std::string &userId,
                                                              const std::optional<PageCursor> &after, int limit);

    boost::asio::awaitable<Result<std::optional<Receipt>>> update(const std::string &userId, const std::string &id,
                                                                  const ReceiptChanges &changes);

    // Soft delete: the row stays (with deleted_at set) but disappears from the API.
    boost::asio::awaitable<Result<bool>> remove(const std::string &userId, const std::string &id);

    boost::asio::awaitable<Result<std::optional<Receipt>>> attachImage(const std::string &userId, const std::string &id,
                                                                       const ReceiptImage &image);

    // For the Obsidian sync worker, which acts on behalf of all users.
    boost::asio::awaitable<Result<std::vector<Receipt>>> listNotYetInObsidian(int limit);
    boost::asio::awaitable<Result<void>> markInObsidian(const std::string &id);

private:
    db::PgPool &pool_;
};

} // namespace accounts::receipts
