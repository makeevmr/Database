#ifndef DATABASE_PARSE_QUERY
#define DATABASE_PARSE_QUERY

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

void skipUntil(const std::string &query, size_t &query_index, unsigned char symbol);

void skipSpaces(const std::string &query, size_t &query_index);

void pushBackColumnName(const std::string &query, std::string &column_name, bool &column_name_formed,
                        unsigned char symbol);

void parseTableName(const std::string &query, std::string &table_name, size_t &query_index);

void parseValues(const std::string &query, const std::string &table_name, std::vector<std::string> &column_names_array,
                 std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map,
                 size_t &query_index);

void parseCreateQuery(const std::string &query,
                      std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map);

void parseInsertQuery(const std::string &query,
                      std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map);

void parseUpdateQuery(const std::string &query,
                      std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map);

void parseDropQuery(const std::string &query,
                    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map);

void updateColumnSize(const std::string &query,
                      std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map);

#endif
