#include "llama_model.h"
#include "../utils/logger.h"

// Include llama.cpp headers
#include "../../external/llama.cpp/include/llama.h"
#include "../../external/llama.cpp/common/common.h"

#include <sstream>
#include <thread>
#include <atomic>

namespace tarius::models
{
    // Private implementation struct to hide llama.cpp details
    struct LlamaModel::PrivateImplementation
    {
        llama_model *model = nullptr;
        llama_context *ctx = nullptr;
        const llama_vocab *vocab = nullptr;
        llama_sampler *sampler = nullptr;

        ~PrivateImplementation()
        {
            if (sampler)
            {
                llama_sampler_free(sampler);
                sampler = nullptr;
            }
            if (ctx)
            {
                llama_free(ctx);
                ctx = nullptr;
            }
            if (model)
            {
                llama_model_free(model);
                model = nullptr;
            }
        }
    };

    /**
     * @brief Constructs a LlamaModel with the specified configuration.
     *
     * @param config The model configuration containing model path, context size, etc.
     */
    LlamaModel::LlamaModel(const ModelConfig &config)
        : m_config(config), m_initialized(false), m_impl(std::make_unique<PrivateImplementation>())
    {
    }

    /**
     * @brief Destructor for LlamaModel.
     * Uses default implementation which properly cleans up the PrivateImplementation.
     */
    LlamaModel::~LlamaModel() = default;

    /**
     * @brief Initializes the LLaMA model and its context.
     *
     * Loads the model from the specified path, creates a context with the configured
     * parameters, and sets up a greedy sampler for text generation.
     *
     * @return true if initialization was successful, false otherwise.
     */
    bool LlamaModel::initialize()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_initialized)
        {
            LOG_INFO("Model already initialized");
            return true;
        }

        LOG_INFO("Initializing LlamaModel with model: {}", m_config.model_path);

        // Initialize llama.cpp backends
        ggml_backend_load_all();

        // Model parameters
        llama_model_params model_params = llama_model_default_params();
        model_params.n_gpu_layers = 99; // Use as many GPU layers as possible

        // Load the model
        m_impl->model = llama_model_load_from_file(m_config.model_path.c_str(), model_params);
        if (!m_impl->model)
        {
            LOG_ERROR("Failed to load model from {}", m_config.model_path);
            return false;
        }

        // Get the vocabulary
        m_impl->vocab = llama_model_get_vocab(m_impl->model);

        // Context parameters
        llama_context_params ctx_params = llama_context_default_params();
        ctx_params.n_ctx = m_config.context_size;
        ctx_params.n_threads = m_config.threads;
        ctx_params.n_threads_batch = m_config.threads;

        // Create context
        m_impl->ctx = llama_init_from_model(m_impl->model, ctx_params);
        if (!m_impl->ctx)
        {
            LOG_ERROR("Failed to create context");
            llama_model_free(m_impl->model);
            m_impl->model = nullptr;
            return false;
        }

        // Initialize the sampler with default chain
        auto sparams = llama_sampler_chain_default_params();
        m_impl->sampler = llama_sampler_chain_init(sparams);

        // Add a greedy sampler (simplest option)
        llama_sampler_chain_add(m_impl->sampler, llama_sampler_init_greedy());
        // llama_sampler_chain_add(m_impl->sampler, llama_sampler_init_top_k(40));
        // llama_sampler_chain_add(m_impl->sampler, llama_sampler_init_top_p(0.9, 0.05));
        // llama_sampler_chain_add(m_impl->sampler, llama_sampler_init_temperature(0.7));

        LOG_INFO("Model initialized successfully");
        m_initialized = true;
        return true;
    }

    // std::string LlamaModel::generate(const std::string &prompt)
    // {
    //     std::lock_guard<std::mutex> lock(m_mutex);

    //     if (!m_initialized)
    //     {
    //         LOG_ERROR("Model not initialized");
    //         return "Error: Model not initialized";
    //     }

    //     // Commented out for now as it's too verbose
    //     // LOG_INFO("Generating text for prompt: {}", prompt);

    //     // Prepare the full prompt (system + user prompt)
    //     std::string full_prompt = prompt;
    //     if (!m_config.system_prompt.empty())
    //     {
    //         full_prompt = m_config.system_prompt + "\n" + prompt;
    //     }

    //     // Tokenize the prompt
    //     std::vector<llama_token> tokens;

    //     // Get the count of tokens
    //     int n_tokens = -llama_tokenize(m_impl->vocab, full_prompt.c_str(), full_prompt.length(), nullptr, 0, true, true);
    //     if (n_tokens <= 0)
    //     {
    //         LOG_ERROR("Failed to count tokens");
    //         return "Error: Failed to tokenize prompt";
    //     }

    //     // Resize the vector and tokenize
    //     tokens.resize(n_tokens);
    //     if (llama_tokenize(m_impl->vocab, full_prompt.c_str(), full_prompt.length(), tokens.data(), tokens.size(), true, true) < 0)
    //     {
    //         LOG_ERROR("Failed to tokenize prompt");
    //         return "Error: Failed to tokenize prompt";
    //     }

    //     // Prepare a batch for the prompt
    //     llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());

    //     // Evaluate the prompt
    //     if (llama_decode(m_impl->ctx, batch))
    //     {
    //         LOG_ERROR("Failed to decode prompt");
    //         return "Error: Failed to decode prompt";
    //     }

    //     // Generate the response
    //     std::stringstream ss;
    //     llama_token new_token_id;
    //     int n_predict = 0;

    //     while (n_predict < m_config.n_predict)
    //     {
    //         // Sample the next token
    //         new_token_id = llama_sampler_sample(m_impl->sampler, m_impl->ctx, -1);

    //         // Check for end of generation
    //         if (llama_vocab_is_eog(m_impl->vocab, new_token_id))
    //         {
    //             break;
    //         }

    //         // Convert token to text
    //         char buf[128];
    //         int n = llama_token_to_piece(m_impl->vocab, new_token_id, buf, sizeof(buf), 0, true);
    //         if (n < 0)
    //         {
    //             LOG_ERROR("Failed to convert token to piece");
    //             break;
    //         }

    //         // Append to result
    //         ss.write(buf, n);

    //         // Prepare next batch with the new token
    //         batch = llama_batch_get_one(&new_token_id, 1);

    //         // Decode the token
    //         if (llama_decode(m_impl->ctx, batch))
    //         {
    //             LOG_ERROR("Failed to decode token");
    //             break;
    //         }

    //         n_predict++;
    //     }

    //     return ss.str();
    // }

    /**
     * @brief Generates text based on the provided prompt.
     *
     * Tokenizes the input prompt, processes it through the model, and generates
     * a response using the configured sampler. Stops generation when reaching
     * max tokens or encountering a stop sequence.
     *
     * @param prompt The input text to generate a response for.
     * @return The generated text response.
     */
    std::string LlamaModel::generate(const std::string &prompt)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_initialized)
        {
            LOG_ERROR("Model not initialized");
            return "Error: Model not initialized";
        }

        // Prepare the full prompt using ChatML format
        std::string full_prompt;
        if (!m_config.system_prompt.empty())
        {
            full_prompt = "<|system|>\n" + m_config.system_prompt + "\n</|system|>\n<|user|>\n" + prompt + "\n</|user|>\n<|assistant|>\n";
        }
        else
        {
            full_prompt = "<|user|>\n" + prompt + "\n</|user|>\n<|assistant|>\n";
        }

        // log out the full prompt with '====' before and after
        LOG_INFO("\n\nFull prompt with History\n====\n{}\n====\n\n", full_prompt);

        // Check context length before tokenizing
        int estimated_tokens = full_prompt.length() / 4; // rough estimate
        if (estimated_tokens > m_config.context_size)
        {
            LOG_ERROR("Prompt likely too long for context window");
        }

        // Tokenize the prompt
        std::vector<llama_token> tokens;

        // Get the count of tokens
        int n_tokens = -llama_tokenize(m_impl->vocab, full_prompt.c_str(), full_prompt.length(), nullptr, 0, true, true);
        if (n_tokens <= 0)
        {
            LOG_ERROR("Failed to count tokens");
            return "Error: Failed to tokenize prompt";
        }

        LOG_INFO("Tokenized prompt length: {} tokens (context size: {})", n_tokens, m_config.context_size);

        // Resize the vector and tokenize
        tokens.resize(n_tokens);
        if (llama_tokenize(m_impl->vocab, full_prompt.c_str(), full_prompt.length(), tokens.data(), tokens.size(), true, true) < 0)
        {
            LOG_ERROR("Failed to tokenize prompt");
            return "Error: Failed to tokenize prompt";
        }

        // Prepare a batch for the prompt
        llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());

        // Evaluate the prompt
        if (llama_decode(m_impl->ctx, batch))
        {
            LOG_ERROR("Failed to decode prompt");
            return "Error: Failed to decode prompt";
        }

        // Generate the response
        std::stringstream ss;
        std::string buffer; // Buffer to check for stop sequences
        llama_token new_token_id;
        int n_predict = 0;

        const std::vector<std::string> stop_sequences = {
            // ChatML format markers
            "<|system|>", "</|system|>", "<|user|>", "</|user|>", "<|assistant|>", "</|assistant|>",
            // User/assistant markers
            "User:", "Wee Hung:", "Tarius:", "You:", "Human:",
            // Common model regeneration patterns
            "System:", "Assistant:", "AI:", "Model:",
            // Other harmful leaks
            "system prompt", "System Prompt", "SYSTEM PROMPT"};

        while (n_predict < m_config.n_predict)
        {
            // Sample the next token
            new_token_id = llama_sampler_sample(m_impl->sampler, m_impl->ctx, -1);

            // Check for end of generation
            if (llama_vocab_is_eog(m_impl->vocab, new_token_id))
            {
                break;
            }

            // Convert token to text
            char buf[128];
            int n = llama_token_to_piece(m_impl->vocab, new_token_id, buf, sizeof(buf), 0, true);
            if (n < 0)
            {
                LOG_ERROR("Failed to convert token to piece");
                break;
            }

            // Append to result and buffer
            ss.write(buf, n);
            buffer += std::string(buf, n);

            // Check if any stop sequence is found
            bool should_stop = false;
            for (const auto &stop_seq : stop_sequences)
            {
                if (buffer.find(stop_seq) != std::string::npos)
                {
                    should_stop = true;
                    // Trim the stop sequence from the output
                    std::string result = ss.str();
                    size_t pos = result.find(stop_seq);
                    if (pos != std::string::npos)
                    {
                        result = result.substr(0, pos);
                    }
                    return result;
                }
            }
            if (should_stop)
                break;

            // Keep buffer size manageable (only need to check last N characters)
            if (buffer.length() > 20)
            { // 20 is more than longest stop sequence
                buffer = buffer.substr(buffer.length() - 20);
            }

            // Prepare next batch with the new token
            batch = llama_batch_get_one(&new_token_id, 1);

            // Decode the token
            if (llama_decode(m_impl->ctx, batch))
            {
                LOG_ERROR("Failed to decode token");
                break;
            }

            n_predict++;
        }

        return ss.str();
    }

    /**
     * @brief Checks if the model has been successfully initialized.
     *
     * @return true if the model is initialized, false otherwise.
     */
    bool LlamaModel::isInitialized() const
    {
        return m_initialized;
    }

    /**
     * @brief Summarizes a conversation using the LLM.
     *
     * Creates a prompt asking the model to summarize the provided conversation
     * and returns the generated summary.
     *
     * @param conversation The conversation text to summarize.
     * @return A summary of the conversation.
     */
    std::string LlamaModel::summariseConversation(const std::string &conversation)
    {
        std::string prompt = "Summarise the following conversation: " + conversation;
        return generate(prompt);
    }
} // namespace tarius::models