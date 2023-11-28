#include "Authorization/Authorization.h"
#include "LoadDB/LoadDB.h"
#include "ParseQuery/ParseQuery.h"
#include "ParseTableToStr/ParseTableToStr.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define TABLES_MAP_PATH "config/TablesMap.json"
#define LOG_FILE_PATH "log/LogFile.txt"

int main() {
    bool is_db_modified = false;
    bool is_super_user = false;
    std::string sql_query;
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> tables_map;
    std::unordered_map<std::string, std::string> main_db_config_map;
    std::unordered_map<std::string, std::string> user_db_config_map;
    std::ifstream config_json;
    size_t response_size = 1024;
    unsigned char *response = new unsigned char[response_size];
    try {
        loadConfig(config_json, main_db_config_map, 1);
        loadConfig(config_json, user_db_config_map, 2);
        pqxx::connection main_db_connection(getConnectionQuery(main_db_config_map));
        pqxx::connection user_db_connection(getConnectionQuery(user_db_config_map));
        if (main_db_connection.is_open() && user_db_connection.is_open()) {
            std::ofstream log_file(LOG_FILE_PATH, std::ios::app);
            if (!log_file.is_open()) {
                throw UnopenableFileError(std::string("Error. Couldn't open file: ") + std::string(LOG_FILE_PATH));
            }
            is_super_user = authorization(user_db_connection, log_file);
            std::ifstream input_table_map_json(TABLES_MAP_PATH);
            if (input_table_map_json.is_open()) {
                readTablesMapFromJson(input_table_map_json, tables_map);
            } else {
                throw UnopenableFileError(std::string("Error. Couldn't open file: ") + std::string(TABLES_MAP_PATH));
            }
            input_table_map_json.close();
            std::cout << "Opened database successfully: " << main_db_connection.dbname() << std::endl;
            // configure the server
            int server_sfd, connection_sfd, port_num;
            socklen_t client_addr_size;
            const size_t query_buffer_size = 256;
            char query_buffer[query_buffer_size];
            struct sockaddr_in server_addr, client_addr;
            int readed_bytes;
            server_sfd = socket(AF_INET, SOCK_STREAM, 0);
            if (server_sfd < 0) {
                throw UnopenableSocketError();
            }
            bzero((char *)&server_addr, sizeof(server_addr));
            port_num = 2024;
            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = INADDR_ANY;
            server_addr.sin_port = htons(port_num);
            if (bind(server_sfd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
                throw BindSocketError();
            }
            listen(server_sfd, 5);
            client_addr_size = sizeof(client_addr);
            connection_sfd = accept(server_sfd, reinterpret_cast<struct sockaddr *>(&client_addr), &client_addr_size);
            while (true) {
                try {
                    // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    // std::cout << ">> ";
                    // std::getline(std::cin, sql_query, ';');
                    bzero(query_buffer, query_buffer_size);
                    readed_bytes = read(connection_sfd, query_buffer, query_buffer_size - 1);
                    if (readed_bytes < 0) {
                        throw ReadFromSocketError();
                    }
                    std::string sql_query = std::string(query_buffer);
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
                                throw UnopenableFileError(std::string("Error. Couldn't open file: ") +
                                                          std::string(TABLES_MAP_PATH));
                            }
                            output_table_map_json.close();
                        }
                        delete[] response;
                        close(connection_sfd);
                        close(server_sfd);
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
                        size_t response_index = 0;
                        response = parseTableToStr(data_from_query, tables_map[table_name], response, response_index,
                                                   response_size);
                        response[response_index] = '\0';
                        readed_bytes = write(connection_sfd, response, response_index);
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
                throw UnopenableDataBaseError(std::string("Couldn't open database: ") +
                                              std::string(main_db_connection.dbname()));
            } else {
                throw UnopenableDataBaseError(std::string("Couldn't open database: ") +
                                              std::string(user_db_connection.dbname()));
            }
        }
    } catch (const std::exception &e) {
        delete[] response;
        if (is_db_modified) {
            std::ofstream output_table_map_json(TABLES_MAP_PATH);
            if (output_table_map_json.is_open()) {
                writeTablesMapToJson(output_table_map_json, tables_map);
            } else {
                throw UnopenableFileError(std::string("Error. Couldn't open file: ") + std::string(TABLES_MAP_PATH));
            }
            output_table_map_json.close();
        }
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
