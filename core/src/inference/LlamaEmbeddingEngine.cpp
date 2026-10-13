#include "inference/LlamaEmbeddingEngine.hpp"

#include "inference/VectorMath.hpp"

#include <llama.h>

#include <algorithm>
#include <cstdio>
#include <mutex>
#include <filesystem>
#include <thread>

namespace receipts::inference {

namespace {

// Receipt lines are short; 512 tokens leaves plenty of room for the task prefix.
constexpr int contextTokens = 512;

void initialiseLlamaOnce() {
    static std::once_flag once;
    std::call_once(once, [] {
        llama_backend_init();
        // llama.cpp is chatty on stderr by default; keep only errors.
        llama_log_set(
            [](ggml_log_level level, const char *text, void *) {
                if (level == GGML_LOG_LEVEL_ERROR) {
                    std::fputs(text, stderr);
                }
            },
            nullptr);
    });
}

int threadCount() {
    // Leave a core for the UI; at most 4, beyond that small models gain little.
    const unsigned cores = std::max(1u, std::thread::hardware_concurrency());
    return static_cast<int>(std::clamp(cores - 1, 1u, 4u));
}

} // namespace

Result<std::unique_ptr<LlamaEmbeddingEngine>> LlamaEmbeddingEngine::load(const std::string &modelPath) {
    if (!std::filesystem::exists(modelPath)) {
        return fail(ErrorCode::ModelMissing, "embedding model not found: " + modelPath);
    }
    initialiseLlamaOnce();

    llama_model_params modelParams = llama_model_default_params();
    modelParams.n_gpu_layers = 999;  // everything on the GPU where there is one (Metal); ignored on CPU builds
    llama_model *model = llama_model_load_from_file(modelPath.c_str(), modelParams);
    if (model == nullptr) {
        return fail(ErrorCode::ModelMissing, "cannot load embedding model " + modelPath);
    }

    llama_context_params contextParams = llama_context_default_params();
    contextParams.embeddings = true;  // we want vectors, not next-token logits
    contextParams.n_ctx = contextTokens;
    contextParams.n_batch = contextTokens;
    contextParams.n_ubatch = contextTokens;  // a non-causal model must see a whole text in one micro-batch
    contextParams.n_threads = threadCount();
    contextParams.n_threads_batch = threadCount();
    // pooling_type stays "unspecified": the GGUF says how to pool (mean, for EmbeddingGemma).
    llama_context *context = llama_init_from_model(model, contextParams);
    if (context == nullptr) {
        llama_model_free(model);
        return fail(ErrorCode::Internal, "cannot create a llama.cpp context for " + modelPath);
    }
    return std::unique_ptr<LlamaEmbeddingEngine>(
        new LlamaEmbeddingEngine(model, context, std::filesystem::path(modelPath).filename().string()));
}

LlamaEmbeddingEngine::LlamaEmbeddingEngine(llama_model *model, llama_context *context, std::string modelId)
    : model_(model), context_(context), modelId_(std::move(modelId)) {}

LlamaEmbeddingEngine::~LlamaEmbeddingEngine() {
    llama_free(context_);
    llama_model_free(model_);
}

Result<std::vector<Embedding>> LlamaEmbeddingEngine::embed(const std::vector<std::string> &texts) {
    std::lock_guard lock(mutex_);
    std::vector<Embedding> embeddings;
    embeddings.reserve(texts.size());
    for (const std::string &text : texts) {
        auto embedding = embedOne(text);
        if (!embedding) {
            return std::unexpected(embedding.error());
        }
        embeddings.push_back(std::move(*embedding));
    }
    return embeddings;
}

Result<Embedding> LlamaEmbeddingEngine::embedOne(const std::string &text) {
    const llama_vocab *vocab = llama_model_get_vocab(model_);
    std::vector<llama_token> tokens(contextTokens);
    const int tokenCount = llama_tokenize(vocab, text.data(), static_cast<int32_t>(text.size()), tokens.data(),
                                          static_cast<int32_t>(tokens.size()), /*add_special=*/true,
                                          /*parse_special=*/false);
    if (tokenCount <= 0) {
        return fail(ErrorCode::InvalidInput, "text is empty or longer than " + std::to_string(contextTokens) +
                                                 " tokens");
    }
    tokens.resize(tokenCount);

    // Each text is independent: forget the previous one.
    llama_memory_clear(llama_get_memory(context_), true);
    if (llama_decode(context_, llama_batch_get_one(tokens.data(), tokenCount)) != 0) {
        return fail(ErrorCode::Internal, "llama.cpp failed to embed a text");
    }

    // The pooled vector for sequence 0 (the only one in our batch).
    const float *pooled = llama_get_embeddings_seq(context_, 0);
    if (pooled == nullptr) {
        return fail(ErrorCode::Internal, "the model produced no pooled embedding (is it an embedding model?)");
    }
    Embedding embedding(pooled, pooled + llama_model_n_embd_out(model_));
    normalize(embedding);
    return embedding;
}

} // namespace receipts::inference
