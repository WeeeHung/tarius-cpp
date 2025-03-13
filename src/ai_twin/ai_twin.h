#pragma once

#include "../models/memory_manager.h"
#include "../models/llama_model.h"
#include <string>
#include <memory>

namespace tarius::ai_twin
{

    class AITwin
    {
    public:
        AITwin();
        ~AITwin();

        std::string generateResponse(const std::string &userInput);
        bool initializeLlamaModel(const std::string &modelPath);
        bool isLlamaModelInitialized() const;

    private:
        std::unique_ptr<models::MemoryManager> m_memoryManager;
        std::unique_ptr<models::LlamaModel> m_llamaModel;
        bool m_useLlamaModel;

        // For MVP, we'll use a simple approach to generate responses
        // when the LLM is not available
        std::string generateSimpleResponse(const std::string &userInput);

        // Helper methods
        std::string createPrompt(const std::string &userInput);
    };

} // namespace tarius::ai_twin