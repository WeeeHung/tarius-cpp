#pragma once

#include <string>
#include <unordered_map>
// Add any other necessary includes

namespace tarius::utils
{

    class Config
    {
    public:
        Config();
        ~Config();

        bool load();
        bool save();

        // Getters
        std::string getString(const std::string &key, const std::string &defaultValue = "") const;
        int getInt(const std::string &key, int defaultValue = 0) const;
        double getDouble(const std::string &key, double defaultValue = 0.0) const;
        bool getBool(const std::string &key, bool defaultValue = false) const;

        // Setters
        void setString(const std::string &key, const std::string &value);
        void setInt(const std::string &key, int value);
        void setDouble(const std::string &key, double value);
        void setBool(const std::string &key, bool value);

    private:
        std::string m_configFilePath;
        std::unordered_map<std::string, std::string> m_configValues;

        void setDefaults();
    };

} // namespace tarius::utils