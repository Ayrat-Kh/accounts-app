#include "ocr/TesseractOcrEngine.hpp"

#include "util/Text.hpp"

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

namespace receipts::ocr {

namespace {

// Photos are scaled to about this width before OCR. Measured on
// testdata/receipts: small images read markedly better when enlarged to it,
// and huge phone photos (4000 px) only cost time beyond it.
constexpr int targetWidthForOcr = 2400;
constexpr int maximumWidthForOcr = 3200;

// Leptonica images are C objects freed through a pointer-to-pointer.
struct PixDeleter {
    void operator()(Pix *pix) const { pixDestroy(&pix); }
};
using PixPtr = std::unique_ptr<Pix, PixDeleter>;

// Greyscale, scaled to a good width for OCR, deskewed. Each step returns a new image.
PixPtr prepareForOcr(PixPtr image) {
    PixPtr grey(pixConvertTo8(image.get(), 0));
    if (!grey) {
        return image;
    }
    const int width = pixGetWidth(grey.get());
    if (width < targetWidthForOcr || width > maximumWidthForOcr) {
        const float factor = static_cast<float>(targetWidthForOcr) / static_cast<float>(width);
        if (PixPtr scaled(pixScale(grey.get(), factor, factor)); scaled) {
            grey = std::move(scaled);
        }
    }
    if (PixPtr deskewed(pixDeskew(grey.get(), 0)); deskewed) {
        grey = std::move(deskewed);
    }
    return grey;
}

} // namespace

Result<std::unique_ptr<TesseractOcrEngine>> TesseractOcrEngine::create(const std::string &tessdataDirectory,
                                                                       const std::string &languages) {
    auto api = std::make_unique<tesseract::TessBaseAPI>();
    if (api->Init(tessdataDirectory.c_str(), languages.c_str(), tesseract::OEM_LSTM_ONLY) != 0) {
        return fail(ErrorCode::ModelMissing,
                    "cannot load Tesseract languages '" + languages + "' from " + tessdataDirectory);
    }
    // One uniform block of text. The column-detecting modes treat the wide
    // gap before a receipt's price column as a column break and separate
    // every price from its label ("TOTAL" ... "10.64"); this mode keeps
    // "TOTAL              10.64" on one line.
    api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
    // Keep runs of spaces: "Milk      1,99" stays one readable line.
    api->SetVariable("preserve_interword_spaces", "1");
    return std::unique_ptr<TesseractOcrEngine>(new TesseractOcrEngine(std::move(api)));
}

TesseractOcrEngine::TesseractOcrEngine(std::unique_ptr<tesseract::TessBaseAPI> api) : api_(std::move(api)) {}

TesseractOcrEngine::~TesseractOcrEngine() {
    api_->End();
}

Result<OcrPage> TesseractOcrEngine::recognize(const std::string &imagePath) {
    PixPtr image(pixRead(imagePath.c_str()));
    if (!image) {
        return fail(ErrorCode::InvalidInput, "cannot decode image " + imagePath);
    }
    PixPtr prepared = prepareForOcr(std::move(image));

    std::lock_guard lock(mutex_);
    api_->SetImage(prepared.get());
    if (api_->Recognize(nullptr) != 0) {
        return fail(ErrorCode::Internal, "Tesseract could not recognise " + imagePath);
    }

    OcrPage page;
    page.imageHeight = pixGetHeight(prepared.get());
    std::unique_ptr<tesseract::ResultIterator> line(api_->GetIterator());
    if (!line) {
        return page;  // a blank image: no text at all
    }
    do {
        std::unique_ptr<char[]> utf8(line->GetUTF8Text(tesseract::RIL_TEXTLINE));
        if (!utf8) {
            continue;
        }
        const std::string text = text::trim(utf8.get());
        if (text.empty()) {
            continue;
        }
        int left = 0, top = 0, right = 0, bottom = 0;
        line->BoundingBox(tesseract::RIL_TEXTLINE, &left, &top, &right, &bottom);
        page.lines.push_back({text, top, bottom - top, line->Confidence(tesseract::RIL_TEXTLINE)});
    } while (line->Next(tesseract::RIL_TEXTLINE));
    api_->Clear();
    return page;
}

} // namespace receipts::ocr
