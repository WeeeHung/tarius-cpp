#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>

namespace tarius::models
{
    /**
     * @brief A wrapper class for the llama.cpp library.
     *
     * This class provides a simplified interface to the llama.cpp library
     * for generating text from a prompt.
     */
    class LlamaModel
    {
    public:
        // Configuration for the model
        struct ModelConfig
        {
            std::string model_path;         // Path to the model file
            int context_size = 2048;        // Context size for the model
            int threads = 4;                // Number of threads to use
            int n_predict = 256;            // Maximum number of tokens to predict
            float temperature = 0.8f;       // Sampling temperature
            int top_k = 40;                 // Top-k sampling parameter
            float top_p = 0.9f;             // Top-p sampling parameter
            std::string system_prompt = ""; // System prompt to use
        };

        /**
         * @brief Constructor
         *
         * @param config Configuration for the model
         */
        LlamaModel(const ModelConfig &config);

        /**
         * @brief Destructor
         */
        ~LlamaModel();

        /**
         * @brief Initialize the model with the given configuration.
         *
         * @return true if initialization was successful, false otherwise
         */
        bool initialize();

        /**
         * @brief Generate a response to the given prompt.
         *
         * @param prompt The prompt to generate a response for
         * @return The generated response
         */
        std::string generate(const std::string &prompt);

        /**
         * @brief Check if the model has been initialized.
         *
         * @return true if the model has been initialized, false otherwise
         */
        bool isInitialized() const;

        /**
         * @brief Summarise a conversation.
         *
         * @param conversation The conversation to summarise
         * @return The summarised conversation
         */
        std::string summariseConversation(const std::string &conversation);

    private:
        ModelConfig m_config;
        bool m_initialized;

        // Forward declarations for llama.cpp types to avoid including the headers
        struct PrivateImplementation;
        std::unique_ptr<PrivateImplementation> m_impl;

        // Mutex for thread safety
        std::mutex m_mutex;
    };

} // namespace tarius::models