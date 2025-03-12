#include "memory_manager.h"
#include "../utils/logger.h"
#include "../utils/json_handler.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace tarius::models
{

    // Message serialization
    std::string Message::toJson() const
    {
        json j;
        j["speaker"] = speaker;
        j["content"] = content;

        // Convert timestamp to ISO string
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
        j["timestamp"] = ss.str();

        return j.dump();
    }

    Message Message::fromJson(const std::string &jsonStr)
    {
        json j = json::parse(jsonStr);
        Message msg;
        msg.speaker = j["speaker"];
        msg.content = j["content"];

        // Parse timestamp
        std::tm tm = {};
        std::stringstream ss(j["timestamp"].get<std::string>());
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        msg.timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        return msg;
    }

    // Conversation serialization
    std::string Conversation::toJson() const
    {
        json j;
        j["id"] = id;

        // Convert start time to ISO string
        time_t startTimeT = std::chrono::system_clock::to_time_t(startTime);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&startTimeT), "%Y-%m-%dT%H:%M:%S");
        j["startTime"] = ss.str();

        // Serialize messages
        json messagesJson = json::array();
        for (const auto &msg : messages)
        {
            messagesJson.push_back(json::parse(msg.toJson()));
        }
        j["messages"] = messagesJson;

        return j.dump(4); // Pretty print with 4 spaces
    }

    Conversation Conversation::fromJson(const std::string &jsonStr)
    {
        json j = json::parse(jsonStr);
        Conversation conv;
        conv.id = j["id"];

        // Parse start time
        std::tm tm = {};
        std::stringstream ss(j["startTime"].get<std::string>());
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        conv.startTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        // Parse messages
        for (const auto &msgJson : j["messages"])
        {
            Message msg;
            msg.speaker = msgJson["speaker"];
            msg.content = msgJson["content"];

            // Parse timestamp
            std::tm msgTm = {};
            std::stringstream msgSs(msgJson["timestamp"].get<std::string>());
            msgSs >> std::get_time(&msgTm, "%Y-%m-%dT%H:%M:%S");
            msg.timestamp = std::chrono::system_clock::from_time_t(std::mktime(&msgTm));

            conv.messages.push_back(msg);
        }

        return conv;
    }

    // Summary serialization
    std::string Summary::toJson() const
    {
        json j;
        j["conversationId"] = conversationId;
        j["content"] = content;

        // Convert timestamp to ISO string
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
        j["timestamp"] = ss.str();

        return j.dump(4); // Pretty print with 4 spaces
    }

    Summary Summary::fromJson(const std::string &jsonStr)
    {
        json j = json::parse(jsonStr);
        Summary summary;
        summary.conversationId = j["conversationId"];
        summary.content = j["content"];

        // Parse timestamp
        std::tm tm = {};
        std::stringstream ss(j["timestamp"].get<std::string>());
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        summary.timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        return summary;
    }

    // MemoryManager implementation
    MemoryManager::MemoryManager()
    {
        // Create necessary directories if they don't exist
        fs::create_directories("data/conversations");
        fs::create_directories("data/summaries");

        // Start a new conversation
        startNewConversation();
    }

    MemoryManager::~MemoryManager()
    {
        // Save current conversation before shutting down
        saveCurrentConversation();
    }

    void MemoryManager::addMessage(const std::string &speaker, const std::string &content)
    {
        Message msg;
        msg.speaker = speaker;
        msg.content = content;
        msg.timestamp = std::chrono::system_clock::now();

        m_currentConversation.messages.push_back(msg);

        // Auto-save after each message
        saveCurrentConversation();
    }

    void MemoryManager::saveCurrentConversation()
    {
        if (m_currentConversation.messages.empty())
        {
            return; // Don't save empty conversations
        }

        saveConversation(m_currentConversation);
    }

    void MemoryManager::startNewConversation()
    {
        // Save the current conversation if it exists
        saveCurrentConversation();

        // Create a new conversation
        m_currentConversation.id = generateConversationId();
        m_currentConversation.startTime = std::chrono::system_clock::now();
        m_currentConversation.messages.clear();

        LOG_INFO("Started new conversation with ID: {}", m_currentConversation.id);
    }

    std::vector<Message> MemoryManager::getRecentMessages(int count)
    {
        std::vector<Message> recentMessages;

        // First, get messages from current conversation
        auto currentMessages = m_currentConversation.messages;

        // If we need more messages than what's in the current conversation,
        // we could load previous conversations here

        // Return the most recent 'count' messages
        int startIdx = std::max(0, static_cast<int>(currentMessages.size()) - count);
        for (int i = startIdx; i < currentMessages.size(); i++)
        {
            recentMessages.push_back(currentMessages[i]);
        }

        return recentMessages;
    }

    std::vector<Conversation> MemoryManager::getConversations(const std::string &dateFrom, const std::string &dateTo)
    {
        std::vector<Conversation> conversations;

        // Parse date strings to time_points
        std::tm fromTm = {}, toTm = {};
        std::stringstream fromSs(dateFrom), toSs(dateTo);
        fromSs >> std::get_time(&fromTm, "%Y-%m-%d");
        toSs >> std::get_time(&toTm, "%Y-%m-%d");

        auto fromTime = std::chrono::system_clock::from_time_t(std::mktime(&fromTm));
        auto toTime = std::chrono::system_clock::from_time_t(std::mktime(&toTm));

        // Iterate through conversation files
        for (const auto &entry : fs::directory_iterator("data/conversations"))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                Conversation conv;
                if (loadConversation(entry.path().stem().string(), conv))
                {
                    // Check if conversation is within date range
                    if (conv.startTime >= fromTime && conv.startTime <= toTime)
                    {
                        conversations.push_back(conv);
                    }
                }
            }
        }

        return conversations;
    }

    void MemoryManager::summarizeConversation(const std::string &conversationId)
    {
        Conversation conv;
        if (!loadConversation(conversationId, conv))
        {
            LOG_ERROR("Failed to load conversation for summarization: {}", conversationId);
            return;
        }

        // In a real implementation, this would use the AI to generate a summary
        // For MVP, we'll just create a simple summary
        std::stringstream summaryContent;
        time_t startTimeT = std::chrono::system_clock::to_time_t(conv.startTime);
        summaryContent << "Summary of conversation on "
                       << std::put_time(std::localtime(&startTimeT), "%Y-%m-%d %H:%M:%S")
                       << ":\n\n";

        summaryContent << "This conversation had " << conv.messages.size() << " messages.\n";
        summaryContent << "Topics discussed: [Would be generated by AI in full implementation]";

        Summary summary;
        summary.conversationId = conversationId;
        summary.content = summaryContent.str();
        summary.timestamp = std::chrono::system_clock::now();

        saveSummary(summary);
        LOG_INFO("Created summary for conversation: {}", conversationId);
    }

    void MemoryManager::summarizeOldConversations(int daysOld)
    {
        auto now = std::chrono::system_clock::now();
        auto cutoffTime = now - std::chrono::hours(24 * daysOld);

        for (const auto &entry : fs::directory_iterator("data/conversations"))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                Conversation conv;
                if (loadConversation(entry.path().stem().string(), conv))
                {
                    // Check if conversation is older than cutoff
                    if (conv.startTime < cutoffTime)
                    {
                        // Check if summary already exists
                        std::string summaryPath = getSummaryPath(conv.id);
                        if (!fs::exists(summaryPath))
                        {
                            summarizeConversation(conv.id);
                        }
                    }
                }
            }
        }
    }

    std::vector<Summary> MemoryManager::getSummaries(const std::string &dateFrom, const std::string &dateTo)
    {
        std::vector<Summary> summaries;

        // Parse date strings to time_points
        std::tm fromTm = {}, toTm = {};
        std::stringstream fromSs(dateFrom), toSs(dateTo);
        fromSs >> std::get_time(&fromTm, "%Y-%m-%d");
        toSs >> std::get_time(&toTm, "%Y-%m-%d");

        auto fromTime = std::chrono::system_clock::from_time_t(std::mktime(&fromTm));
        auto toTime = std::chrono::system_clock::from_time_t(std::mktime(&toTm));

        // Iterate through summary files
        for (const auto &entry : fs::directory_iterator("data/summaries"))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                std::ifstream file(entry.path());
                if (file.is_open())
                {
                    std::string jsonStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    file.close();

                    Summary summary = Summary::fromJson(jsonStr);

                    // Check if summary is within date range
                    if (summary.timestamp >= fromTime && summary.timestamp <= toTime)
                    {
                        summaries.push_back(summary);
                    }
                }
            }
        }

        return summaries;
    }

    std::string MemoryManager::generateConversationId()
    {
        // Generate a timestamp-based ID
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << "conv_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        return ss.str();
    }

    std::string MemoryManager::getConversationPath(const std::string &id)
    {
        return "data/conversations/" + id + ".json";
    }

    std::string MemoryManager::getSummaryPath(const std::string &id)
    {
        return "data/summaries/" + id + "_summary.json";
    }

    bool MemoryManager::loadConversation(const std::string &id, Conversation &conversation)
    {
        std::string path = getConversationPath(id);
        std::ifstream file(path);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open conversation file: {}", path);
            return false;
        }

        std::string jsonStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        try
        {
            conversation = Conversation::fromJson(jsonStr);
            return true;
        }
        catch (const std::exception &e)
        {
            LOG_ERROR("Failed to parse conversation JSON: {}", e.what());
            return false;
        }
    }

    bool MemoryManager::saveConversation(const Conversation &conversation)
    {
        std::string path = getConversationPath(conversation.id);
        std::ofstream file(path);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open conversation file for writing: {}", path);
            return false;
        }

        file << conversation.toJson();
        file.close();

        LOG_INFO("Saved conversation: {}", conversation.id);
        return true;
    }

    bool MemoryManager::saveSummary(const Summary &summary)
    {
        std::string path = getSummaryPath(summary.conversationId);
        std::ofstream file(path);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open summary file for writing: {}", path);
            return false;
        }

        file << summary.toJson();
        file.close();

        LOG_INFO("Saved summary for conversation: {}", summary.conversationId);
        return true;
    }

} // namespace tarius::models