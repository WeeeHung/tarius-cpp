#include "config.h"
#include "logger.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace tarius::utils
{

    Config::Config()
        : m_configFilePath("data/config.json")
    {
    }

    Config::~Config() = default;

    bool Config::load()
    {
        // Check if file exists
        if (!fs::exists(m_configFilePath))
        {
            LOG_INFO("Config file does not exist, creating with defaults");
            setDefaults();
            return save();
        }

        // Read file
        std::ifstream file(m_configFilePath);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open config file for reading: {}", m_configFilePath);
            setDefaults();
            return false;
        }

        try
        {
            json configJson = json::parse(file);
            file.close();

            // Clear existing values
            m_configValues.clear();

            // Load values from JSON
            for (auto it = configJson.begin(); it != configJson.end(); ++it)
            {
                m_configValues[it.key()] = it.value().dump();
            }

            LOG_INFO("Loaded configuration from {}", m_configFilePath);
            return true;
        }
        catch (const std::exception &e)
        {
            LOG_ERROR("Failed to parse config file: {}", e.what());
            setDefaults();
            return false;
        }
    }

    bool Config::save()
    {
        // Create directory if it doesn't exist
        fs::create_directories(fs::path(m_configFilePath).parent_path());

        // Create JSON object
        json configJson;

        // Add values to JSON
        for (const auto &pair : m_configValues)
        {
            try
            {
                configJson[pair.first] = json::parse(pair.second);
            }
            catch (const std::exception &)
            {
                // If parsing fails, store as string
                configJson[pair.first] = pair.second;
            }
        }

        // Write to file
        std::ofstream file(m_configFilePath);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open config file for writing: {}", m_configFilePath);
            return false;
        }

        file << configJson.dump(4);
        file.close();

        LOG_INFO("Saved configuration to {}", m_configFilePath);
        return true;
    }

    std::string Config::getString(const std::string &key, const std::string &defaultValue) const
    {
        auto it = m_configValues.find(key);
        if (it != m_configValues.end())
        {
            try
            {
                return json::parse(it->second).get<std::string>();
            }
            catch (const std::exception &)
            {
                // If parsing fails, return the raw value
                return it->second;
            }
        }
        return defaultValue;
    }

    int Config::getInt(const std::string &key, int defaultValue) const
    {
        auto it = m_configValues.find(key);
        if (it != m_configValues.end())
        {
            try
            {
                return json::parse(it->second).get<int>();
            }
            catch (const std::exception &)
            {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    double Config::getDouble(const std::string &key, double defaultValue) const
    {
        auto it = m_configValues.find(key);
        if (it != m_configValues.end())
        {
            try
            {
                return json::parse(it->second).get<double>();
            }
            catch (const std::exception &)
            {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    bool Config::getBool(const std::string &key, bool defaultValue) const
    {
        auto it = m_configValues.find(key);
        if (it != m_configValues.end())
        {
            try
            {
                return json::parse(it->second).get<bool>();
            }
            catch (const std::exception &)
            {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    void Config::setString(const std::string &key, const std::string &value)
    {
        m_configValues[key] = json(value).dump();
    }

    void Config::setInt(const std::string &key, int value)
    {
        m_configValues[key] = json(value).dump();
    }

    void Config::setDouble(const std::string &key, double value)
    {
        m_configValues[key] = json(value).dump();
    }

    void Config::setBool(const std::string &key, bool value)
    {
        m_configValues[key] = json(value).dump();
    }

    void Config::setDefaults()
    {
        // Set default configuration values
        setString("user.name", "User");
        setString("ai.name", "Tarius");
        setBool("ai.proactive_reminders", true);
        setInt("memory.max_recent_messages", 10);
        setInt("memory.summarize_after_days", 1);
    }

} // namespace tarius::utils