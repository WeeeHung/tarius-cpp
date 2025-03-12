#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace tarius::models
{

    struct Message
    {
        std::string speaker;
        std::string content;
        std::chrono::system_clock::time_point timestamp;

        // For serialization
        std::string toJson() const;
        static Message fromJson(const std::string &json);
    };

    struct Conversation
    {
        std::string id;
        std::vector<Message> messages;
        std::chrono::system_clock::time_point startTime;

        // For serialization
        std::string toJson() const;
        static Conversation fromJson(const std::string &json);
    };

    struct Summary
    {
        std::string conversationId;
        std::string content;
        std::chrono::system_clock::time_point timestamp;

        // For serialization
        std::string toJson() const;
        static Summary fromJson(const std::string &json);
    };

    class MemoryManager
    {
    public:
        MemoryManager();
        ~MemoryManager();

        // Conversation management
        void addMessage(const std::string &speaker, const std::string &content);
        void saveCurrentConversation();
        void startNewConversation();

        // Retrieval
        std::vector<Message> getRecentMessages(int count = 10);
        std::vector<Conversation> getConversations(const std::string &dateFrom, const std::string &dateTo);

        // Summarization
        void summarizeConversation(const std::string &conversationId);
        void summarizeOldConversations(int daysOld = 1);
        std::vector<Summary> getSummaries(const std::string &dateFrom, const std::string &dateTo);

    private:
        Conversation m_currentConversation;
        std::string generateConversationId();
        std::string getConversationPath(const std::string &id);
        std::string getSummaryPath(const std::string &id);

        // Helper methods
        bool loadConversation(const std::string &id, Conversation &conversation);
        bool saveConversation(const Conversation &conversation);
        bool saveSummary(const Summary &summary);
    };

} // namespace tarius::models