#pragma once

#include "calendar.h"
#include "task_list.h"
#include <string>
#include <vector>
#include <memory>

namespace tarius::ai_secretary
{

    class AISecretary
    {
    public:
        AISecretary();
        ~AISecretary();

        bool isSecretaryTask(const std::string &input);
        std::string handleTask(const std::string &input);
        std::vector<std::string> getActiveReminders();

    private:
        std::unique_ptr<Calendar> m_calendar;
        std::unique_ptr<TaskList> m_taskList;

        // Task detection and handling
        bool isSchedulingTask(const std::string &input);
        bool isReminderTask(const std::string &input);
        bool isSummaryTask(const std::string &input);

        // Task execution
        std::string handleScheduling(const std::string &input);
        std::string handleReminder(const std::string &input);
        std::string handleSummary(const std::string &input);

        // Helper methods
        std::string extractDate(const std::string &input);
        std::string extractTime(const std::string &input);
        std::string extractEventName(const std::string &input);
    };

} // namespace tarius::ai_secretary