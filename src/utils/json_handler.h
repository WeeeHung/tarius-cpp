#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace tarius::utils
{

    class JsonHandler
    {
    public:
        static bool saveToFile(const std::string &filePath, const nlohmann::json &data);
        static bool loadFromFile(const std::string &filePath, nlohmann::json &data);
    };

} // namespace tarius::utils