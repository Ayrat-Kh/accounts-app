#pragma once

#include <chrono>
#include <optional>
#include <string>

namespace receipts {

// Where a model runs.
struct EngineConfig {
    enum class Kind {
        Local,             // in-process llama.cpp with a GGUF file (phones, offline Macs)
        OpenAiCompatible,  // an HTTP server: llama.cpp's llama-server, oMLX, Ollama, LM Studio...
    };

    Kind kind = Kind::Local;
    std::string modelFile = "embeddinggemma-2-Q8_0.gguf";  // Local: file name inside the models directory
    std::string baseUrl;                                   // OpenAiCompatible: e.g. "http://localhost:8080"
    std::string model;                                     // OpenAiCompatible: the server's model name
    std::optional<std::string> apiKeySecureStoreKey;       // OpenAiCompatible: SecureStore key holding a bearer key
    // Embedding models are trained with task prefixes; change it together
    // with the model. For EmbeddingGemma 2, the "search result" prompt
    // separated totals from other lines best on testdata/receipts (better
    // than "classification" and "sentence similarity").
    std::string textPrefix = "task: search result | query: ";
    std::chrono::seconds timeout{30};
};

struct CoreConfig {
    std::string apiBaseUrl = "https://accounts.fernfall.cloud";
    // Writable directory owned by the library: local database, photos, models.
    std::string dataDirectory;
    // The user's usual currency (ISO 4217), for receipts that print none.
    std::string defaultCurrency;
    // Tesseract languages, '+'-separated; one <lang>.traineddata each.
    std::string ocrLanguages = "eng+deu+rus";
    // Directory of trusted CA certificates for HTTPS. Android has no default
    // bundle that OpenSSL finds, so the app passes "/system/etc/security/cacerts".
    std::optional<std::string> caCertificatesDirectory;

    // Scores receipt lines ("is this the total?"). Without it (or without its
    // model file), extraction falls back to keyword matching.
    std::optional<EngineConfig> embedding = EngineConfig{};
    // Optional generative model, asked only when the embedding result is
    // unsure. Typically a larger model on a Mac (oMLX or llama-server).
    std::optional<EngineConfig> completion;
    double askCompletionBelowConfidence = 0.6;
};

} // namespace receipts
