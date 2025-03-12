#include "ai_twin.h"
#include "../utils/logger.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <random>
#include <iomanip>

namespace tarius::ai_twin
{

    AITwin::AITwin()
        : m_memoryManager(std::make_unique<models::MemoryManager>())
    {
    }

    AITwin::~AITwin() = default;

    std::string AITwin::generateResponse(const std::string &userInput)
    {
        // Log the user input
        m_memoryManager->addMessage("user", userInput);

        // Generate a response
        std::string response = generateSimpleResponse(userInput);

        // Log the AI response
        m_memoryManager->addMessage("ai", response);

        return response;
    }

    std::string AITwin::generateSimpleResponse(const std::string &userInput)
    {
        // For MVP, we'll use a simple rule-based approach
        // In a full implementation, this would call an LLM API or use a local model

        // Convert input to lowercase for easier matching
        std::string input = userInput;
        std::transform(input.begin(), input.end(), input.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });

        // Simple response patterns
        if (input.find("hello") != std::string::npos ||
            input.find("hi") != std::string::npos)
        {
            return "Hello! How can I assist you today?";
        }

        if (input.find("how are you") != std::string::npos)
        {
            return "I'm functioning well, thank you for asking! How are you doing?";
        }

        if (input.find("name") != std::string::npos &&
            input.find("your") != std::string::npos)
        {
            return "I'm Tarius, your personal AI twin. I'm designed to assist you and learn from our interactions.";
        }

        if (input.find("thank") != std::string::npos)
        {
            return "You're welcome! I'm happy to help.";
        }

        if (input.find("bye") != std::string::npos ||
            input.find("goodbye") != std::string::npos)
        {
            return "Goodbye! Feel free to chat again anytime.";
        }

        if (input.find("weather") != std::string::npos)
        {
            return "I don't have access to real-time weather data in this MVP version, but in the future, I'll be able to provide weather forecasts for you.";
        }

        if (input.find("time") != std::string::npos)
        {
            time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::stringstream ss;
            ss << "The current time is " << std::put_time(std::localtime(&current_time), "%H:%M:%S");
            return ss.str();
        }

        if (input.find("date") != std::string::npos)
        {
            time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::stringstream ss;
            ss << "Today's date is " << std::put_time(std::localtime(&current_time), "%Y-%m-%d");
            return ss.str();
        }

        if (input.find("joke") != std::string::npos)
        {
            std::vector<std::string> jokes = {
                "Why don't scientists trust atoms? Because they make up everything!",
                "Why did the scarecrow win an award? Because he was outstanding in his field!",
                "I told my wife she was drawing her eyebrows too high. She looked surprised.",
                "What do you call a fake noodle? An impasta!",
                "How does a computer get drunk? It takes screenshots!"};

            // Select a random joke
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, jokes.size() - 1);
            return jokes[distrib(gen)];
        }

        if (input.find("help") != std::string::npos)
        {
            return "I can help you with various tasks like scheduling events, setting reminders, "
                   "and having conversations. Just tell me what you need!";
        }

        // Default responses for when no pattern matches
        std::vector<std::string> defaultResponses = {
            "I understand what you're saying. Can you tell me more?",
            "That's interesting. How can I help you with that?",
            "I'm still learning, but I'd like to understand more about what you need.",
            "Could you provide more details so I can assist you better?",
            "I'm here to help. What specifically would you like me to do?"};

        // Select a random default response
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, defaultResponses.size() - 1);
        return defaultResponses[distrib(gen)];
    }

    std::string AITwin::createPrompt(const std::string &userInput)
    {
        // This method would be used in a full implementation to create a prompt for an LLM
        // For MVP, we're using simple pattern matching instead

        // Get recent conversation history
        auto recentMessages = m_memoryManager->getRecentMessages(5);

        std::stringstream prompt;
        prompt << "You are Tarius, a personal AI assistant. Respond to the following conversation:\n\n";

        // Add conversation history to the prompt
        for (const auto &msg : recentMessages)
        {
            prompt << (msg.speaker == "user" ? "User: " : "Tarius: ") << msg.content << "\n";
        }

        // Add the current user input
        prompt << "User: " << userInput << "\n";
        prompt << "Tarius: ";

        return prompt.str();
    }

} // namespace tarius::ai_twin