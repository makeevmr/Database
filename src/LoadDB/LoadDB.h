#ifndef DATABASE_LOAD_CONFIG
#define DATABASE_LOAD_CONFIG

#define MAIN_DB_CONFIG_PATH "config/MainDBConfig.json"
#define USER_DB_CONFIG_PATH "config/UserDBConfig.json"

#include "../JsonHandle/JsonHandle.h"

void loadConfig(std::ifstream &input_config, std::unordered_map<std::string, std::string> &config_map, bool is_main);

std::string getConnectionQuery(std::unordered_map<std::string, std::string> &config_map);

#endif // DATABASE_LOAD_CONFIG
