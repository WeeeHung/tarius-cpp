#pragma once

#include "../models/memory_manager.h"
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

    private:
        std::unique_ptr<models::MemoryManager> m_memoryManager;

        // For MVP, we'll use a simple approach to generate responses
        // In a full implementation, this would connect to an LLM
        std::string generateSimpleResponse(const std::string &userInput);

        // Helper methods
        std::string createPrompt(const std::string &userInput);
    };

} // namespace tarius::ai_twin