#pragma once

// Photo -> ScanResult: OCR, then price extraction. The whole on-device pipeline.

#include "extract/PriceExtractor.hpp"
#include "ocr/OcrEngine.hpp"

namespace receipts::extract {

class ReceiptScanner {
public:
    ReceiptScanner(ocr::OcrEngine &ocr, PriceExtractor &extractor) : ocr_(ocr), extractor_(extractor) {}

    Result<ScanResult> scan(const std::string &imagePath) {
        auto page = ocr_.recognize(imagePath);
        if (!page) {
            return std::unexpected(page.error());
        }
        return extractor_.extract(*page);
    }

private:
    ocr::OcrEngine &ocr_;
    PriceExtractor &extractor_;
};

} // namespace receipts::extract
