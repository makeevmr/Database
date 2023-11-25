#ifndef DATA_BASE_AUTHORIZATION
#define DATA_BASE_AUTHORIZATION

#include "../Error.h"
#include "../ParseQuery/ParseQuery.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <pqxx/pqxx>

std::time_t getCurrentTime();

bool authorization(pqxx::connection &user_db_connection, std::ofstream &log_file);

void giveAccess(pqxx::connection &user_db_connection, std::string &query);

#endif
