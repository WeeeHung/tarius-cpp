#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace tarius::ai_secretary
{

    class Calendar
    {
    public:
        struct Event
        {
            std::string title;
            std::chrono::system_clock::time_point time;
            std::string description;
            bool isAllDay = false;
        };

        Calendar();
        ~Calendar();

        void addEvent(const Event &event);
        void removeEvent(const std::string &title);
        std::vector<Event> getEvents(const std::string &date);
        std::vector<Event> getEventsForTime(const std::chrono::system_clock::time_point &time);
        void saveEvents();
        void loadEvents();

    private:
        std::vector<Event> m_events;
        std::string m_calendarFilePath;
    };

} // namespace tarius::ai_secretary