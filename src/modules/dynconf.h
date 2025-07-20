#ifndef DYNCONF_H
#define DYNCONF_H

#include <string>
#include <map>

class ConfigManager {    
public:
    static inline bool isEnabled(std::string module, bool defaultValue = true) {
        auto it = getInstance().content.find(module);
        if (it == getInstance().content.end())
            return defaultValue;
        return (*it).second;
    }

    static inline void initConfig(std::string path) {
        getInstance().init(path);
    }
    static inline ConfigManager& getInstance() {
        static ConfigManager manager;
        return manager;
    }

    ConfigManager(ConfigManager const&) = delete; // Prevent copying
    void operator=(ConfigManager const&) = delete; // Prevent assignment

private:
    ConfigManager() {}
    ~ConfigManager() {}

    std::map<std::string, bool> content;

    void init(std::string path);
};

#endif /* DYNCONF_H */