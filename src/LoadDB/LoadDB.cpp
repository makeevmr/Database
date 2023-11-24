#include "LoadDB.h"


void loadConfig(std::ifstream &input_config, std::unordered_map<std::string, std::string> &config_map, bool is_main) {
    if (is_main) {
        input_config.open(MAIN_DB_CONFIG_PATH);
    } else {
        input_config.open(USER_DB_CONFIG_PATH);
    }
    if (input_config.is_open()) {
        readConfigMapFromJson(input_config, config_map);
    }
    input_config.close();
}

std::string getConnectionQuery(std::unordered_map<std::string, std::string> &config_map) {
    return "dbname = " + config_map["dbname"] + " user = " + config_map["user"] +
           " password = " + config_map["password"] + " hostaddr = " + config_map["hostaddr"] +
           " port = " + config_map["port"];
}
