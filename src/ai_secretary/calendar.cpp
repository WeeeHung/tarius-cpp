#include "calendar.h"
#include "../utils/logger.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace tarius::ai_secretary
{

    Calendar::Calendar()
        : m_calendarFilePath("data/calendar/events.json")
    {
        loadEvents();
    }

    Calendar::~Calendar()
    {
        saveEvents();
    }

    void Calendar::addEvent(const Event &event)
    {
        m_events.push_back(event);
        saveEvents();
        LOG_INFO("Added event: {}", event.title);
    }

    void Calendar::removeEvent(const std::string &title)
    {
        auto it = std::remove_if(m_events.begin(), m_events.end(),
                                 [&title](const Event &event)
                                 { return event.title == title; });

        if (it != m_events.end())
        {
            m_events.erase(it, m_events.end());
            saveEvents();
            LOG_INFO("Removed event: {}", title);
        }
        else
        {
            LOG_WARN("Event not found: {}", title);
        }
    }

    std::vector<Calendar::Event> Calendar::getEvents(const std::string &date)
    {
        std::vector<Event> events;

        // Parse the date string
        std::tm tm = {};
        std::stringstream ss(date);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        auto targetDate = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        // Get the start and end of the target date
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        auto startOfDay = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        tm.tm_hour = 23;
        tm.tm_min = 59;
        tm.tm_sec = 59;
        auto endOfDay = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        // Find events on the target date
        for (const auto &event : m_events)
        {
            if (event.time >= startOfDay && event.time <= endOfDay)
            {
                events.push_back(event);
            }
        }

        return events;
    }

    std::vector<Calendar::Event> Calendar::getEventsForTime(const std::chrono::system_clock::time_point &time)
    {
        std::vector<Event> events;

        // Convert time to time_t for easier comparison
        auto timeT = std::chrono::system_clock::to_time_t(time);
        std::tm tm = *std::localtime(&timeT);

        // Round to the nearest minute
        tm.tm_sec = 0;
        auto roundedTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        // Find events that match the current time (within a minute)
        for (const auto &event : m_events)
        {
            auto eventTimeT = std::chrono::system_clock::to_time_t(event.time);
            std::tm eventTm = *std::localtime(&eventTimeT);
            eventTm.tm_sec = 0;
            auto roundedEventTime = std::chrono::system_clock::from_time_t(std::mktime(&eventTm));

            if (roundedEventTime == roundedTime)
            {
                events.push_back(event);
            }
        }

        return events;
    }

    void Calendar::saveEvents()
    {
        // Create directory if it doesn't exist
        fs::create_directories("data/calendar");

        // Create JSON array
        json eventsJson = json::array();

        for (const auto &event : m_events)
        {
            json eventJson;
            eventJson["title"] = event.title;
            eventJson["description"] = event.description;
            eventJson["isAllDay"] = event.isAllDay;

            // Convert time to ISO string
            auto time_t = std::chrono::system_clock::to_time_t(event.time);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
            eventJson["time"] = ss.str();

            eventsJson.push_back(eventJson);
        }

        // Write to file
        std::ofstream file(m_calendarFilePath);
        if (file.is_open())
        {
            file << eventsJson.dump(4);
            file.close();
            LOG_INFO("Saved {} events to calendar", m_events.size());
        }
        else
        {
            LOG_ERROR("Failed to open calendar file for writing: {}", m_calendarFilePath);
        }
    }

    void Calendar::loadEvents()
    {
        // Clear existing events
        m_events.clear();

        // Check if file exists
        if (!fs::exists(m_calendarFilePath))
        {
            LOG_INFO("Calendar file does not exist, starting with empty calendar");
            return;
        }

        // Read file
        std::ifstream file(m_calendarFilePath);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open calendar file for reading: {}", m_calendarFilePath);
            return;
        }

        try
        {
            json eventsJson = json::parse(file);
            file.close();

            for (const auto &eventJson : eventsJson)
            {
                Event event;
                event.title = eventJson["title"];
                event.description = eventJson["description"];
                event.isAllDay = eventJson["isAllDay"];

                // Parse time
                std::tm tm = {};
                std::stringstream ss(eventJson["time"].get<std::string>());
                ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
                event.time = std::chrono::system_clock::from_time_t(std::mktime(&tm));

                m_events.push_back(event);
            }

            LOG_INFO("Loaded {} events from calendar", m_events.size());
        }
        catch (const std::exception &e)
        {
            LOG_ERROR("Failed to parse calendar file: {}", e.what());
        }
    }

} // namespace tarius::ai_secretary