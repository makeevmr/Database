#ifndef DATABASE_PARSE_QUERY
#define DATABASE_PARSE_QUERY

#include <string>
#include <unordered_map>
#include <vector>

inline void skipUntil(const std::string &query, size_t &query_index, unsigned char symbol);

inline void skipSpaces(const std::string &query, size_t &query_index);

inline void pushBackColumnName(std::string &column_name, bool &column_name_formed, unsigned char symbol, bool modify);

inline void parseTableName(const std::string &query, std::string &table_name, size_t &query_index);

inline void parseValues(const std::string &query, const std::string &table_name,
                        std::vector<std::string> &column_names_array,
                        std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map,
                        size_t &query_index);

inline void parseSequence(const std::string &query, size_t &query_index, std::vector<std::string> &names_array,
                          bool modify);

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

std::string getTableNameSelectQuery(const std::string &query);

void parseUsers(const std::string &query, std::vector<std::string> &username_array);

bool isAcceptableQuery(const std::string &command);

#endif // DATABASE_PARSE_QUERY
