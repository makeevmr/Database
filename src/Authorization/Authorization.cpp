#include "Authorization.h"

std::time_t getCurrentTime() {
    const auto time_point = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(time_point);
}

bool authorization(pqxx::connection &user_db_connection, int connection_sfd, std::ofstream &log_file,
                   unsigned char connection_type) {
    int readed_bytes = 0;
    char cstr_username[BUFFER_SIZE];
    char cstr_password[BUFFER_SIZE];
    readed_bytes = read(connection_sfd, cstr_username, BUFFER_SIZE - 1);
    cstr_username[readed_bytes] = '\0';
    readed_bytes = read(connection_sfd, cstr_password, BUFFER_SIZE - 1);
    cstr_password[readed_bytes] = '\0';
    std::string username = std::string(cstr_username);
    std::string password = std::string(cstr_password);
    if (connection_type == '1') {
        const std::time_t current_time = getCurrentTime();
        log_file << std::ctime(&current_time) << "Sign up. Username: " << cstr_username
                 << ". Password: " << cstr_password << '\n';
        std::string sql_query = "INSERT INTO USERS (USERNAME, PASSWORD, IS_SUPERUSER) VALUES ('" + username + "', '" +
                                password + "', FALSE);";
        pqxx::work update_db_query(user_db_connection);
        update_db_query.exec(sql_query);
        update_db_query.commit();
        std::string response = "User signed up successfully: " + username + '\n';
        readed_bytes = write(connection_sfd, "000", 3);
        readed_bytes = write(connection_sfd, response.c_str(), response.size());
        return false;
    } else {
        const std::time_t current_time = getCurrentTime();
        log_file << std::ctime(&current_time) << "Sign in. Username: " << cstr_username
                 << ". Password: " << cstr_password << '\n';
        std::string sql_query =
            "SELECT IS_SUPERUSER FROM USERS WHERE (USERNAME = '" + username + "' AND PASSWORD = '" + password + "');";
        pqxx::nontransaction select_query(user_db_connection);
        pqxx::result data_from_query(select_query.exec(sql_query));
        if (data_from_query.size() == 0) {
            throw LoginDataBaseError();
        }
        std::string response = "User signed in successfully: " + username + '\n';
        readed_bytes = write(connection_sfd, "000", 3);
        readed_bytes = write(connection_sfd, response.c_str(), response.size());
        return data_from_query[0][0].c_str()[0] == 't';
    }
}

void giveAccess(pqxx::connection &user_db_connection, std::string &query) {
    std::vector<std::string> username_array;
    parseUsers(query, username_array);
    std::string sql_query_start = "UPDATE USERS SET IS_SUPERUSER = TRUE WHERE USERNAME = '";
    size_t username_arry_size = username_array.size();
    for (size_t username_arry_index = 0; username_arry_index < username_arry_size; ++username_arry_index) {
        std::string sql_query = sql_query_start + username_array[username_arry_index] + "';";
        pqxx::work update_db_query(user_db_connection);
        update_db_query.exec(sql_query);
        update_db_query.commit();
    }
}
