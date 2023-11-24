#ifndef DATABASE_JSON_HANDLE
#define DATABASE_JSON_HANDLE

#include <fstream>
#include <unordered_map>

inline void writeIndent(std::ofstream &output_json, unsigned short int layers);

void writeTablesMapToJson(std::ofstream &output_json,
                          const std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map);

void readTablesMapFromJson(std::ifstream &input_json,
                           std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map);

void readConfigMapFromJson(std::ifstream &input_json, std::unordered_map<std::string, std::string> &config_map);

#endif // DATABASE_JSON_HANDLE
