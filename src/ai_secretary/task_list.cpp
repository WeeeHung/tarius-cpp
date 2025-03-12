#include "task_list.h"
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

    TaskList::TaskList()
        : m_taskFilePath("data/tasks/tasks.json")
    {
        loadTasks();
    }

    TaskList::~TaskList()
    {
        saveTasks();
    }

    void TaskList::addTask(const Task &task)
    {
        m_tasks.push_back(task);
        saveTasks();
        LOG_INFO("Added task: {}", task.description);
    }

    void TaskList::completeTask(const std::string &description)
    {
        bool found = false;
        for (auto &task : m_tasks)
        {
            if (task.description == description)
            {
                task.completed = true;
                found = true;
                break;
            }
        }

        if (found)
        {
            saveTasks();
            LOG_INFO("Completed task: {}", description);
        }
        else
        {
            LOG_WARN("Task not found: {}", description);
        }
    }

    void TaskList::removeTask(const std::string &description)
    {
        auto it = std::remove_if(m_tasks.begin(), m_tasks.end(),
                                 [&description](const Task &task)
                                 { return task.description == description; });

        if (it != m_tasks.end())
        {
            m_tasks.erase(it, m_tasks.end());
            saveTasks();
            LOG_INFO("Removed task: {}", description);
        }
        else
        {
            LOG_WARN("Task not found: {}", description);
        }
    }

    std::vector<TaskList::Task> TaskList::getAllTasks()
    {
        return m_tasks;
    }

    std::vector<TaskList::Task> TaskList::getDueTasks(const std::chrono::system_clock::time_point &time)
    {
        std::vector<Task> dueTasks;

        // Convert time to time_t for easier comparison
        auto timeT = std::chrono::system_clock::to_time_t(time);
        std::tm tm = *std::localtime(&timeT);

        // Round to the nearest minute
        tm.tm_sec = 0;
        auto roundedTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        // Find tasks that are due at the current time (within a minute)
        for (const auto &task : m_tasks)
        {
            if (task.completed)
            {
                continue; // Skip completed tasks
            }

            auto taskTimeT = std::chrono::system_clock::to_time_t(task.dueTime);
            std::tm taskTm = *std::localtime(&taskTimeT);
            taskTm.tm_sec = 0;
            auto roundedTaskTime = std::chrono::system_clock::from_time_t(std::mktime(&taskTm));

            if (roundedTaskTime == roundedTime)
            {
                dueTasks.push_back(task);
            }
        }

        return dueTasks;
    }

    void TaskList::saveTasks()
    {
        // Create directory if it doesn't exist
        fs::create_directories("data/tasks");

        // Create JSON array
        json tasksJson = json::array();

        for (const auto &task : m_tasks)
        {
            json taskJson;
            taskJson["description"] = task.description;
            taskJson["completed"] = task.completed;
            taskJson["priority"] = task.priority;

            // Convert due time to ISO string
            auto time_t = std::chrono::system_clock::to_time_t(task.dueTime);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S");
            taskJson["dueTime"] = ss.str();

            tasksJson.push_back(taskJson);
        }

        // Write to file
        std::ofstream file(m_taskFilePath);
        if (file.is_open())
        {
            file << tasksJson.dump(4);
            file.close();
            LOG_INFO("Saved {} tasks", m_tasks.size());
        }
        else
        {
            LOG_ERROR("Failed to open task file for writing: {}", m_taskFilePath);
        }
    }

    void TaskList::loadTasks()
    {
        // Clear existing tasks
        m_tasks.clear();

        // Check if file exists
        if (!fs::exists(m_taskFilePath))
        {
            LOG_INFO("Task file does not exist, starting with empty task list");
            return;
        }

        // Read file
        std::ifstream file(m_taskFilePath);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open task file for reading: {}", m_taskFilePath);
            return;
        }

        try
        {
            json tasksJson = json::parse(file);
            file.close();

            for (const auto &taskJson : tasksJson)
            {
                Task task;
                task.description = taskJson["description"];
                task.completed = taskJson["completed"];
                task.priority = taskJson["priority"];

                // Parse due time
                std::tm tm = {};
                std::stringstream ss(taskJson["dueTime"].get<std::string>());
                ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
                task.dueTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));

                m_tasks.push_back(task);
            }

            LOG_INFO("Loaded {} tasks", m_tasks.size());
        }
        catch (const std::exception &e)
        {
            LOG_ERROR("Failed to parse task file: {}", e.what());
        }
    }

} // namespace tarius::ai_secretary