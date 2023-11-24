#ifndef DATABASE_VIEW_TABLE
#define DATABASE_VIEW_TABLE

#include <iomanip>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <unordered_map>

void viewTable(const pqxx::result &res, const std::unordered_map<std::string, size_t> &column_map);

#endif // DATABASE_VIEW_TABLE
