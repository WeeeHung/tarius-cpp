#pragma once

#include "../ai_twin/ai_twin.h"
#include "../ai_secretary/ai_secretary.h"
#include <string>
#include <memory>

namespace tarius::app
{

    class AppController
    {
    public:
        AppController();
        ~AppController();

        std::string processUserInput(const std::string &input);
        void checkReminders();

    private:
        std::unique_ptr<ai_twin::AITwin> m_aiTwin;
        std::unique_ptr<ai_secretary::AISecretary> m_aiSecretary;
    };

} // namespace tarius::app