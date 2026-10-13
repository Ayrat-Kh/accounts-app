#pragma once

// In-memory stand-ins for the ports and the network, so the logic can be
// tested without servers, Keychains or model files.

#include "inference/CompletionEngine.hpp"
#include "net/HttpTransport.hpp"
#include "receipts/Ports.hpp"

#include <deque>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace receipts::testing {

class MemorySecureStore : public SecureStore {
public:
    std::optional<std::string> read(const std::string &key) override {
        auto found = values.find(key);
        return found == values.end() ? std::nullopt : std::optional(found->second);
    }
    void write(const std::string &key, const std::string &value) override { values[key] = value; }
    void remove(const std::string &key) override { values.erase(key); }

    std::map<std::string, std::string> values;
};

class RecordingLogger : public Logger {
public:
    void log(LogLevel, std::string_view message) override { messages.emplace_back(message); }
    std::vector<std::string> messages;
};

// Answers requests with a handler, and records what was sent.
class FakeHttpTransport : public net::HttpTransport {
public:
    using Handler = std::function<Result<net::HttpResponse>(const net::HttpRequest &)>;

    explicit FakeHttpTransport(Handler handler) : handler_(std::move(handler)) {}

    Result<net::HttpResponse> send(const net::HttpRequest &request) override {
        std::lock_guard lock(mutex_);
        requests.push_back(request);
        return handler_(request);
    }

    std::vector<net::HttpRequest> requests;

private:
    Handler handler_;
    std::mutex mutex_;
};

inline net::HttpResponse jsonResponse(int status, std::string body) {
    return {status, {{"content-type", "application/json"}}, std::move(body)};
}

class FakeCompletionEngine : public inference::CompletionEngine {
public:
    explicit FakeCompletionEngine(std::string reply) : reply_(std::move(reply)) {}

    Result<std::string> completeJson(const std::string &, const std::string &input, const std::string &) override {
        lastInput = input;
        return reply_;
    }

    std::string lastInput;

private:
    std::string reply_;
};

} // namespace receipts::testing
