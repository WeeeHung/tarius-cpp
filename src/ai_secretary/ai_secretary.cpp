#include "ai_secretary.h"
#include "../utils/logger.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace tarius::ai_secretary
{

    AISecretary::AISecretary()
        : m_calendar(std::make_unique<Calendar>()),
          m_taskList(std::make_unique<TaskList>())
    {
    }

    AISecretary::~AISecretary() = default;

    bool AISecretary::isSecretaryTask(const std::string &input)
    {
        return isSchedulingTask(input) || isReminderTask(input) || isSummaryTask(input);
    }

    std::string AISecretary::handleTask(const std::string &input)
    {
        if (isSchedulingTask(input))
        {
            return handleScheduling(input);
        }
        else if (isReminderTask(input))
        {
            return handleReminder(input);
        }
        else if (isSummaryTask(input))
        {
            return handleSummary(input);
        }

        return "I'm not sure how to handle that task.";
    }

    std::vector<std::string> AISecretary::getActiveReminders()
    {
        // Get current time
        auto now = std::chrono::system_clock::now();

        // Check for calendar events
        auto events = m_calendar->getEventsForTime(now);

        // Check for tasks
        auto tasks = m_taskList->getDueTasks(now);

        // Combine events and tasks into reminders
        std::vector<std::string> reminders;

        for (const auto &event : events)
        {
            reminders.push_back("Event: " + event.title + " is starting now.");
        }

        for (const auto &task : tasks)
        {
            reminders.push_back("Task: " + task.description + " is due now.");
        }

        return reminders;
    }

    bool AISecretary::isSchedulingTask(const std::string &input)
    {
        std::string lowerInput = input;
        std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });

        return (lowerInput.find("schedule") != std::string::npos ||
                lowerInput.find("meeting") != std::string::npos ||
                lowerInput.find("appointment") != std::string::npos ||
                lowerInput.find("event") != std::string::npos) &&
               (lowerInput.find("tomorrow") != std::string::npos ||
                lowerInput.find("today") != std::string::npos ||
                lowerInput.find("next") != std::string::npos ||
                std::regex_search(lowerInput, std::regex("\\b\\d{1,2}(:\\d{2})?\\s*(am|pm)\\b")) ||
                std::regex_search(lowerInput, std::regex("\\b\\d{4}-\\d{2}-\\d{2}\\b")));
    }

    bool AISecretary::isReminderTask(const std::string &input)
    {
        std::string lowerInput = input;
        std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });

        return (lowerInput.find("remind") != std::string::npos ||
                lowerInput.find("remember") != std::string::npos ||
                lowerInput.find("don't forget") != std::string::npos ||
                lowerInput.find("task") != std::string::npos ||
                lowerInput.find("to-do") != std::string::npos ||
                lowerInput.find("todo") != std::string::npos);
    }

    bool AISecretary::isSummaryTask(const std::string &input)
    {
        std::string lowerInput = input;
        std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });

        return (lowerInput.find("summarize") != std::string::npos ||
                lowerInput.find("summary") != std::string::npos) &&
               (lowerInput.find("conversation") != std::string::npos ||
                lowerInput.find("chat") != std::string::npos ||
                lowerInput.find("discussion") != std::string::npos);
    }

    std::string AISecretary::handleScheduling(const std::string &input)
    {
        // Extract date, time, and event name from input
        std::string date = extractDate(input);
        std::string time = extractTime(input);
        std::string eventName = extractEventName(input);

        // Create event
        Calendar::Event event;
        event.title = eventName;

        // Parse date and time
        std::tm tm = {};
        std::stringstream ss;

        if (!date.empty() && !time.empty())
        {
            ss << date << " " << time;
            ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
        }
        else if (!date.empty())
        {
            ss << date << " 12:00"; // Default to noon
            ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
        }
        else
        {
            // Default to today
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            tm = *std::localtime(&time_t);

            if (!time.empty())
            {
                std::stringstream timeSs(time);
                int hour, minute;
                char colon;
                timeSs >> hour >> colon >> minute;
                tm.tm_hour = hour;
                tm.tm_min = minute;
            }
        }

        event.time = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        // Add event to calendar
        m_calendar->addEvent(event);

        // Format response
        std::stringstream response;
        response << "I've scheduled \"" << eventName << "\" for ";
        response << std::put_time(&tm, "%B %d, %Y at %I:%M %p");
        response << ". I'll remind you when it's time.";

        return response.str();
    }

    std::string AISecretary::handleReminder(const std::string &input)
    {
        // Extract task description and due date/time
        std::string date = extractDate(input);
        std::string time = extractTime(input);

        // The task description is everything except the date/time and reminder keywords
        std::string taskDesc = input;

        // Remove reminder keywords
        std::vector<std::string> keywords = {"remind", "remember", "don't forget", "task", "to-do", "todo"};
        for (const auto &keyword : keywords)
        {
            size_t pos = taskDesc.find(keyword);
            if (pos != std::string::npos)
            {
                taskDesc.erase(pos, keyword.length());
            }
        }

        // Remove date and time if they exist
        if (!date.empty())
        {
            size_t pos = taskDesc.find(date);
            if (pos != std::string::npos)
            {
                taskDesc.erase(pos, date.length());
            }
        }

        if (!time.empty())
        {
            size_t pos = taskDesc.find(time);
            if (pos != std::string::npos)
            {
                taskDesc.erase(pos, time.length());
            }
        }

        // Clean up the task description
        taskDesc = std::regex_replace(taskDesc, std::regex("\\s+"), " ");
        taskDesc = std::regex_replace(taskDesc, std::regex("^\\s+|\\s+$"), "");

        // If task description is empty, use a generic one
        if (taskDesc.empty())
        {
            taskDesc = "Unnamed task";
        }

        // Create task
        TaskList::Task task;
        task.description = taskDesc;

        // Parse date and time
        std::tm tm = {};
        std::stringstream ss;

        if (!date.empty() && !time.empty())
        {
            ss << date << " " << time;
            ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
        }
        else if (!date.empty())
        {
            ss << date << " 12:00"; // Default to noon
            ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
        }
        else
        {
            // Default to today
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            tm = *std::localtime(&time_t);

            if (!time.empty())
            {
                std::stringstream timeSs(time);
                int hour, minute;
                char colon;
                timeSs >> hour >> colon >> minute;
                tm.tm_hour = hour;
                tm.tm_min = minute;
            }
        }

        task.dueTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        // Add task to task list
        m_taskList->addTask(task);

        // Format response
        std::stringstream response;
        response << "I'll remind you to \"" << taskDesc << "\" on ";
        response << std::put_time(&tm, "%B %d, %Y at %I:%M %p");

        return response.str();
    }

    std::string AISecretary::handleSummary(const std::string &input)
    {
        // For MVP, we'll just return a simple message
        // In a full implementation, this would use the MemoryManager to retrieve and summarize conversations

        return "I'm still learning how to summarize conversations. This feature will be available in a future update.";
    }

    std::string AISecretary::extractDate(const std::string &input)
    {
        // Look for explicit date format (YYYY-MM-DD)
        std::regex dateRegex("\\b(\\d{4}-\\d{2}-\\d{2})\\b");
        std::smatch match;
        if (std::regex_search(input, match, dateRegex))
        {
            return match[1];
        }

        // Look for relative dates
        std::string lowerInput = input;
        std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });

        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time_t);

        if (lowerInput.find("today") != std::string::npos)
        {
            // Use today's date
        }
        else if (lowerInput.find("tomorrow") != std::string::npos)
        {
            // Add one day
            tm.tm_mday += 1;
            std::mktime(&tm); // Normalize the time
        }
        else if (lowerInput.find("next week") != std::string::npos)
        {
            // Add one week
            tm.tm_mday += 7;
            std::mktime(&tm); // Normalize the time
        }
        else if (lowerInput.find("next month") != std::string::npos)
        {
            // Add one month
            tm.tm_mon += 1;
            std::mktime(&tm); // Normalize the time
        }
        else
        {
            // No date found
            return "";
        }

        // Format the date
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d");
        return ss.str();
    }

    std::string AISecretary::extractTime(const std::string &input)
    {
        // Look for time format (HH:MM)
        std::regex timeRegex("\\b(\\d{1,2}:\\d{2})\\s*(am|pm)?\\b", std::regex_constants::icase);
        std::smatch match;
        if (std::regex_search(input, match, timeRegex))
        {
            std::string time = match[1];
            std::string ampm = match[2];

            // Parse the time
            std::stringstream ss(time);
            int hour, minute;
            char colon;
            ss >> hour >> colon >> minute;

            // Adjust for AM/PM
            if (!ampm.empty())
            {
                std::transform(ampm.begin(), ampm.end(), ampm.begin(),
                               [](unsigned char c)
                               { return std::tolower(c); });

                if (ampm == "pm" && hour < 12)
                {
                    hour += 12;
                }
                else if (ampm == "am" && hour == 12)
                {
                    hour = 0;
                }
            }

            // Format the time
            std::stringstream result;
            result << std::setw(2) << std::setfill('0') << hour << ":"
                   << std::setw(2) << std::setfill('0') << minute;
            return result.str();
        }

        return "";
    }

    std::string AISecretary::extractEventName(const std::string &input)
    {
        // For MVP, we'll use a simple approach
        // Remove scheduling keywords, date, and time to get the event name

        std::string eventName = input;

        // Remove scheduling keywords
        std::vector<std::string> keywords = {"schedule", "meeting", "appointment", "event"};
        for (const auto &keyword : keywords)
        {
            size_t pos = eventName.find(keyword);
            if (pos != std::string::npos)
            {
                eventName.erase(pos, keyword.length());
            }
        }

        // Remove date and time
        std::string date = extractDate(input);
        std::string time = extractTime(input);

        if (!date.empty())
        {
            size_t pos = eventName.find(date);
            if (pos != std::string::npos)
            {
                eventName.erase(pos, date.length());
            }
        }

        if (!time.empty())
        {
            size_t pos = eventName.find(time);
            if (pos != std::string::npos)
            {
                eventName.erase(pos, time.length());
            }
        }

        // Remove common time-related words
        std::vector<std::string> timeWords = {"today", "tomorrow", "next week", "next month", "am", "pm"};
        for (const auto &word : timeWords)
        {
            size_t pos = eventName.find(word);
            if (pos != std::string::npos)
            {
                eventName.erase(pos, word.length());
            }
        }

        // Clean up the event name
        eventName = std::regex_replace(eventName, std::regex("\\s+"), " ");
        eventName = std::regex_replace(eventName, std::regex("^\\s+|\\s+$"), "");

        // If event name is empty, use a generic one
        if (eventName.empty())
        {
            eventName = "Unnamed event";
        }

        return eventName;
    }

} // namespace tarius::ai_secretary