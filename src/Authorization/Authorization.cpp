#include "Authorization.h"

std::time_t getCurrentTime() {
    const auto time_point = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(time_point);
}

bool authorization(pqxx::connection &user_db_connection, std::ofstream &log_file) {
    short int command = 0;
    do {
        std::cout << "Type: 1 or 2\n1 To create a new user\n2 Login with your existing account\n";
        std::cin >> command;
    } while (command != 1 && command != 2);
    std::string username;
    std::string password;
    if (command == 1) {
        std::cout << "Enter username to sign up: ";
        std::cin >> username;
        std::cout << "Enter password to sign up: ";
        std::cin >> password;
        const std::time_t current_time = getCurrentTime();
        log_file << std::ctime(&current_time) << "Sign up. Username: " << username << ". Password: " << password
                 << '\n';
        std::string sql_query = "INSERT INTO USERS (USERNAME, PASSWORD, IS_SUPERUSER) VALUES ('" + username + "', '" +
                                password + "', FALSE);";
        pqxx::work update_db_query(user_db_connection);
        update_db_query.exec(sql_query);
        update_db_query.commit();
        std::cout << "User signed up successfully: " << username << std::endl;
        return false;
    } else {
        std::cout << "Enter username to sign in: ";
        std::cin >> username;
        std::cout << "Enter password to sign in: ";
        std::cin >> password;
        const std::time_t current_time = getCurrentTime();
        log_file << std::ctime(&current_time) << "Sign in. Username: " << username << ". Password: " << password
                 << '\n';
        std::string sql_query =
            "SELECT IS_SUPERUSER FROM USERS WHERE (USERNAME = '" + username + "' AND PASSWORD = '" + password + "');";
        pqxx::nontransaction select_query(user_db_connection);
        pqxx::result data_from_query(select_query.exec(sql_query));
        if (data_from_query.size() == 0) {
            throw LoginDataBaseError();
        }
        std::cout << "User signed in successfully: " << username << std::endl;
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
