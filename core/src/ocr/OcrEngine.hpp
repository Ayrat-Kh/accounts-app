#pragma once

// Image -> lines of text with their positions. TesseractOcrEngine is the
// portable implementation; a platform OCR (Apple Vision, ML Kit) could be
// added behind this interface without touching the price extraction.

#include "receipts/Result.hpp"

#include <string>
#include <vector>

namespace receipts::ocr {

struct OcrLine {
    std::string text;
    int top = 0;  // pixels from the top of the image; receipts put the total near the bottom
    int height = 0;
    float confidence = 0;  // 0..100, as Tesseract reports it
};

struct OcrPage {
    std::vector<OcrLine> lines;  // top to bottom
    int imageHeight = 0;
};

class OcrEngine {
public:
    virtual ~OcrEngine() = default;
    virtual Result<OcrPage> recognize(const std::string &imagePath) = 0;
};

} // namespace receipts::ocr
