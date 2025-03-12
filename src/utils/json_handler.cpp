#include "json_handler.h"
#include "logger.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace tarius::utils
{

    bool JsonHandler::saveToFile(const std::string &filePath, const nlohmann::json &data)
    {
        // Create directory if it doesn't exist
        fs::create_directories(fs::path(filePath).parent_path());

        // Write to file
        std::ofstream file(filePath);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open file for writing: {}", filePath);
            return false;
        }

        file << data.dump(4); // Pretty print with 4 spaces
        file.close();

        return true;
    }

    bool JsonHandler::loadFromFile(const std::string &filePath, nlohmann::json &data)
    {
        // Check if file exists
        if (!fs::exists(filePath))
        {
            LOG_ERROR("File does not exist: {}", filePath);
            return false;
        }

        // Read file
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open file for reading: {}", filePath);
            return false;
        }

        try
        {
            data = nlohmann::json::parse(file);
            file.close();
            return true;
        }
        catch (const std::exception &e)
        {
            LOG_ERROR("Failed to parse JSON file {}: {}", filePath, e.what());
            return false;
        }
    }

} // namespace tarius::utils