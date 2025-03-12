#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace tarius::ai_secretary
{

    class TaskList
    {
    public:
        struct Task
        {
            std::string description;
            std::chrono::system_clock::time_point dueTime;
            bool completed = false;
            int priority = 0; // 0 = normal, 1 = important, 2 = urgent
        };

        TaskList();
        ~TaskList();

        void addTask(const Task &task);
        void completeTask(const std::string &description);
        void removeTask(const std::string &description);
        std::vector<Task> getAllTasks();
        std::vector<Task> getDueTasks(const std::chrono::system_clock::time_point &time);
        void saveTasks();
        void loadTasks();

    private:
        std::vector<Task> m_tasks;
        std::string m_taskFilePath;
    };

} // namespace tarius::ai_secretary