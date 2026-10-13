#pragma once

#include <filesystem>
#include <random>
#include <string>

namespace receipts::testing {

// A fresh directory under the system temp dir, deleted with everything in it
// when the object goes out of scope.
class TempDirectory {
public:
    TempDirectory() {
        std::random_device random;
        path_ = std::filesystem::temp_directory_path() / ("receipts-test-" + std::to_string(random()));
        std::filesystem::create_directories(path_);
    }
    ~TempDirectory() {
        std::error_code ignored;
        std::filesystem::remove_all(path_, ignored);
    }
    TempDirectory(const TempDirectory &) = delete;
    TempDirectory &operator=(const TempDirectory &) = delete;

    const std::filesystem::path &path() const { return path_; }
    std::string file(const std::string &name) const { return (path_ / name).string(); }

private:
    std::filesystem::path path_;
};

} // namespace receipts::testing
