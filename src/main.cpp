#include "Error.h"
#include "LoadDB/LoadDB.h"
#include "ParseQuery/ParseQuery.h"
#include "ViewTable/ViewTable.h"

#define TABLES_MAP_PATH "config/TablesMap.json"

int main() {
    bool is_db_modified = false;
    std::string sql_query;
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> tables_map;
    std::unordered_map<std::string, std::string> main_db_config_map;
    std::unordered_map<std::string, std::string> user_db_config_map;
    std::ifstream main_db_config_json;
    std::ifstream user_db_config_json;
    try {
        loadConfig(main_db_config_json, main_db_config_map, true);
        loadConfig(user_db_config_json, user_db_config_map, false);
        pqxx::connection main_db_connection(getConnectionQuery(main_db_config_map));
        pqxx::connection user_db_connection(getConnectionQuery(user_db_config_map));
        if (main_db_connection.is_open() && user_db_connection.is_open()) {
            std::ifstream input_table_map_json(TABLES_MAP_PATH);
            if (input_table_map_json.is_open()) {
                readTablesMapFromJson(input_table_map_json, tables_map);
            }
            input_table_map_json.close();
            std::cout << "Opened database successfully: " << main_db_connection.dbname() << std::endl;
            while (true) {
                try {
                    std::cout << ">> ";
                    std::getline(std::cin, sql_query, ';');
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    sql_query += ";";
                    if (sql_query == "exit;") {
                        if (is_db_modified) {
                            std::ofstream output_table_map_json(TABLES_MAP_PATH);
                            if (output_table_map_json.is_open()) {
                                writeTablesMapToJson(output_table_map_json, tables_map);
                            }
                            output_table_map_json.close();
                        }
                        main_db_connection.disconnect();
                        break;
                    }
                    if (sql_query.size() < 6) {
                        throw SmallQueryLength();
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
                        is_db_modified = true;
                        pqxx::work update_db_query(main_db_connection);
                        update_db_query.exec(sql_query);
                        update_db_query.commit();
                        if (command.substr(0, 6) != "DELETE") {
                            updateColumnSize(sql_query, tables_map);
                        }
                    }
                    // TODO maybe change exception to std::exception
                } catch (const CustomBaseError &error) {
                    std::cout << error.what() << std::endl;
                }
            }
        } else {
            std::cout << "Can't open database. Check the correctness of the arguments: dbname, user, password"
                      << std::endl;
            return 1;
        }
    } catch (const std::exception &e) {
        if (is_db_modified) {
            std::ofstream output_table_map_json("config/TablesMap.json");
            if (output_table_map_json.is_open()) {
                writeTablesMapToJson(output_table_map_json, tables_map);
            }
            output_table_map_json.close();
        }
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
