
#include <string>
#include <map>

class ConfigManager {
private:
    ConfigManager  () {}
    ~ConfigManager () {}

    std::map<std::string, bool> content;

    void init (std::string path);
    
    
public:
    static bool isEnabled (std::string module, bool defaultValue = true) {
        auto it = getInstance().content.find(module);
        if (it == getInstance().content.end())
            return defaultValue;
        return (*it).second;
    }

    static void initConfig (std::string path) {
        getInstance().init(path);
    }
    static ConfigManager& getInstance () {
        static ConfigManager manager;
        return manager;
    }

    ConfigManager  (ConfigManager const&) = delete; // Prevent copying
    void operator= (ConfigManager const&) = delete; // Prevent assignment
};
