#include "Authorization/Authorization.h"
#include "LoadDB/LoadDB.h"
#include "ParseQuery/ParseQuery.h"
#include "ViewTable/ViewTable.h"

#define TABLES_MAP_PATH "config/TablesMap.json"
#define LOG_FILE_PATH "log/LogFile.txt"
// #define TABLES_MAP_PATH "../config/TablesMap.json"
// TODO delete commented macros later

int main() {
    bool is_db_modified = false;
    bool is_super_user = false;
    std::string sql_query;
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> tables_map;
    std::unordered_map<std::string, std::string> main_db_config_map;
    std::unordered_map<std::string, std::string> user_db_config_map;
    std::ifstream config_json;
    try {
        loadConfig(config_json, main_db_config_map, 1);
        loadConfig(config_json, user_db_config_map, 2);
        pqxx::connection main_db_connection(getConnectionQuery(main_db_config_map));
        pqxx::connection user_db_connection(getConnectionQuery(user_db_config_map));
        if (main_db_connection.is_open() && user_db_connection.is_open()) {
            std::ofstream log_file(LOG_FILE_PATH, std::ios::app);
            if (!log_file.is_open()) {
                throw UnopenableFileError(std::string("Error. Can't open file: ") + std::string(LOG_FILE_PATH));
            }
            is_super_user = authorization(user_db_connection, log_file);
            std::ifstream input_table_map_json(TABLES_MAP_PATH);
            if (input_table_map_json.is_open()) {
                readTablesMapFromJson(input_table_map_json, tables_map);
            } else {
                throw UnopenableFileError(std::string("Error. Can't open file: ") + std::string(TABLES_MAP_PATH));
            }
            input_table_map_json.close();
            std::cout << "Opened database successfully: " << main_db_connection.dbname() << std::endl;
            while (true) {
                try {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << ">> ";
                    std::getline(std::cin, sql_query, ';');
                    sql_query += ";";
                    const std::time_t current_time = getCurrentTime();
                    log_file << std::ctime(&current_time);
                    if (is_super_user) {
                        log_file << "Super user query: " << sql_query << '\n';
                    } else {
                        log_file << "Common user query: " << sql_query << '\n';
                    }
                    if (sql_query == "exit;") {
                        if (is_db_modified) {
                            std::ofstream output_table_map_json(TABLES_MAP_PATH);
                            if (output_table_map_json.is_open()) {
                                writeTablesMapToJson(output_table_map_json, tables_map);
                            } else {
                                throw UnopenableFileError(std::string("Error. Can't open file: ") +
                                                          std::string(TABLES_MAP_PATH));
                            }
                            output_table_map_json.close();
                        }
                        user_db_connection.disconnect();
                        main_db_connection.disconnect();
                        break;
                    }
                    if (sql_query.size() < 6) {
                        throw SmallQueryLengthError();
                    }
                    std::string command = sql_query.substr(0, 6);
                    std::transform(command.begin(), command.end(), command.begin(),
                                   [](unsigned char c) { return std::toupper(c); });
                    if (!isAcceptableQuery(command)) {
                        throw UnsupportedQueryError();
                    }
                    if (sql_query[0] == 'S' || sql_query[0] == 's') {
                        std::transform(sql_query.begin(), sql_query.end(), sql_query.begin(),
                                       [](unsigned char c) { return std::tolower(c); });
                        pqxx::nontransaction select_query(main_db_connection);
                        pqxx::result data_from_query(select_query.exec(sql_query));
                        std::string table_name = getTableNameSelectQuery(sql_query);
                        viewTable(data_from_query, tables_map[table_name]);
                    } else {
                        if (is_super_user) {
                            if (sql_query[0] == 'A' || sql_query[0] == 'a') {
                                giveAccess(user_db_connection, sql_query);
                            } else {
                                is_db_modified = true;
                                pqxx::work update_db_query(main_db_connection);
                                update_db_query.exec(sql_query);
                                update_db_query.commit();
                                if (command.substr(0, 6) != "DELETE") {
                                    updateColumnSize(sql_query, tables_map);
                                }
                            }
                        } else {
                            throw InsufficientAccessRights();
                        }
                    }
                } catch (const CustomBaseError &error) {
                    std::cout << error.what() << std::endl;
                }
            }
        } else {
            if (!main_db_connection.is_open()) {
                throw UnopenableDataBaseError(std::string("Cant't open database: ") +
                                              std::string(main_db_connection.dbname()));
            } else {
                throw UnopenableDataBaseError(std::string("Cant't open database: ") +
                                              std::string(user_db_connection.dbname()));
            }
        }
    } catch (const std::exception &e) {
        if (is_db_modified) {
            std::ofstream output_table_map_json(TABLES_MAP_PATH);
            if (output_table_map_json.is_open()) {
                writeTablesMapToJson(output_table_map_json, tables_map);
            } else {
                throw UnopenableFileError(std::string("Error. Can't open file: ") + std::string(TABLES_MAP_PATH));
            }
            output_table_map_json.close();
        }
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
