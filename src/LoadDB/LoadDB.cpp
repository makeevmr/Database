#include "LoadDB.h"

void loadConfig(std::ifstream &input_config, std::unordered_map<std::string, std::string> &config_map,
                int file_to_open) {
    switch (file_to_open) {
    case 1:
        input_config.open(MAIN_DB_CONFIG_PATH);
        break;
    case 2:
        input_config.open(USER_DB_CONFIG_PATH);
        break;
    case 3:
        input_config.open(ADD_FIRST_USER_CONFIG_PATH);
        break;
    }
    if (input_config.is_open()) {
        readConfigMapFromJson(input_config, config_map);
    } else {
        switch (file_to_open) {
        case 1:
            throw UnopenableFileError(std::string("Error. Couldn't open file: ") + std::string(MAIN_DB_CONFIG_PATH));
            break;
        case 2:
            throw UnopenableFileError(std::string("Error. Couldn't open file: ") + std::string(USER_DB_CONFIG_PATH));
            break;
        case 3:
            throw UnopenableFileError(std::string("Error. Couldn't open file: ") +
                                      std::string(ADD_FIRST_USER_CONFIG_PATH));
        }
    }
    input_config.close();
}

std::string getConnectionQuery(std::unordered_map<std::string, std::string> &config_map) {
    return "dbname = " + config_map["dbname"] + " user = " + config_map["user"] +
           " password = " + config_map["password"] + " hostaddr = " + config_map["hostaddr"] +
           " port = " + config_map["port"];
}
