#include "app_controller.h"
#include "../utils/logger.h"
#include <iostream>

namespace tarius::app
{

    AppController::AppController()
        : m_aiTwin(std::make_unique<ai_twin::AITwin>()), m_aiSecretary(std::make_unique<ai_secretary::AISecretary>())
    {
    }

    AppController::~AppController() = default;

    std::string AppController::processUserInput(const std::string &input)
    {
        LOG_INFO("Processing user input: {}", input);

        // Check if this is a secretary task (scheduling, reminders, etc.)
        if (m_aiSecretary->isSecretaryTask(input))
        {
            return m_aiSecretary->handleTask(input);
        }

        // Otherwise, treat as a conversation with the AI twin
        return m_aiTwin->generateResponse(input);
    }

    void AppController::checkReminders()
    {
        auto reminders = m_aiSecretary->getActiveReminders();
        for (const auto &reminder : reminders)
        {
            std::cout << "\nTarius Reminder: " << reminder << std::endl;
            std::cout << "You: ";
        }
    }

    bool AppController::initializeLlamaModel(const std::string &modelPath)
    {
        LOG_INFO("Initializing LlamaModel from AppController with model path: {}", modelPath);
        return m_aiTwin->initializeLlamaModel(modelPath);
    }

    bool AppController::isLlamaModelInitialized() const
    {
        return m_aiTwin->isLlamaModelInitialized();
    }

} // namespace tarius::app