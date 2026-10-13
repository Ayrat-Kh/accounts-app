#pragma once

// OCR with Tesseract (LSTM engine) and Leptonica for image handling.
//
// Before recognition the photo is converted to greyscale, enlarged if small
// (Tesseract wants text at least ~20 px high) and deskewed: a receipt
// photographed at a slight angle otherwise splits into broken lines.
//
// Tesseract's API object is not thread-safe, so calls are serialised.

#include "ocr/OcrEngine.hpp"

#include <memory>
#include <mutex>

namespace tesseract {
class TessBaseAPI;
}

namespace receipts::ocr {

class TesseractOcrEngine : public OcrEngine {
public:
    // `tessdataDirectory` holds <language>.traineddata files; `languages` is
    // Tesseract's '+'-separated list, e.g. "eng+deu+rus".
    static Result<std::unique_ptr<TesseractOcrEngine>> create(const std::string &tessdataDirectory,
                                                              const std::string &languages);
    ~TesseractOcrEngine() override;

    Result<OcrPage> recognize(const std::string &imagePath) override;

private:
    explicit TesseractOcrEngine(std::unique_ptr<tesseract::TessBaseAPI> api);

    std::unique_ptr<tesseract::TessBaseAPI> api_;
    std::mutex mutex_;
};

} // namespace receipts::ocr
