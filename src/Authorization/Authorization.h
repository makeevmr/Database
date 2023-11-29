#ifndef DATA_BASE_AUTHORIZATION
#define DATA_BASE_AUTHORIZATION

#include "../Error.h"
#include "../ParseQuery/ParseQuery.h"
#include <chrono>
#include <fstream>
#include <pqxx/pqxx>
#include <unistd.h>

#define BUFFER_SIZE 256

std::time_t getCurrentTime();

bool authorization(pqxx::connection &user_db_connection, int connection_sfd, std::ofstream &log_file,
                   unsigned char connection_type);

void giveAccess(pqxx::connection &user_db_connection, std::string &query);

#endif
