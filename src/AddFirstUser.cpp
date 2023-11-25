#include "LoadDB/LoadDB.h"
#include <iostream>
#include <pqxx/pqxx>

int main() {
    std::string sql_query;
    std::unordered_map<std::string, std::string> user_db_config_map;
    std::unordered_map<std::string, std::string> first_user_map;
    std::ifstream config_json;
    try {
        loadConfig(config_json, user_db_config_map, 2);
        loadConfig(config_json, first_user_map, 3);
        pqxx::connection user_db_connection(getConnectionQuery(user_db_config_map));
        if (user_db_connection.is_open()) {
            pqxx::work update_db_query(user_db_connection);
            sql_query = "CREATE TABLE USERS(\
                          ID SERIAL PRIMARY KEY,\
                          USERNAME CHAR(50) UNIQUE NOT NULL,\
                          PASSWORD CHAR(50) UNIQUE NOT NULL,\
                          IS_SUPERUSER BOOLEAN NOT NULL\
                        );";
            update_db_query.exec(sql_query);
            sql_query = "INSERT INTO USERS (USERNAME, PASSWORD, IS_SUPERUSER) VALUES ('" + first_user_map["username"] +
                        "', '" + first_user_map["password"] + "', TRUE);";
            update_db_query.exec(sql_query);
            update_db_query.commit();
        } else {
            throw UnopenableDataBaseError(std::string("Cant't open database: ") +
                                          std::string(user_db_connection.dbname()));
        }
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
