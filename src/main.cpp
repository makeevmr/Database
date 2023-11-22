#include "Error.h"
#include "ParseQuery/ParseQuery.h"
#include "ViewTable/ViewTable.h"
#include <limits>

int main() {
    std::string sql_query;
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> tables_map;
    try {
        pqxx::connection db_connection(
            "dbname = makeevmr user = makeevmr password = Cirjkf281 hostaddr = 127.0.0.1 port = 5432");
        if (db_connection.is_open()) {
            std::cout << "Opened database successfully: " << db_connection.dbname() << std::endl;
            while (true) {
                try {
                    std::cout << ">> ";
                    std::getline(std::cin, sql_query, ';');
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    sql_query += ";";
                    if (sql_query == "exit;") {
                        db_connection.disconnect();
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
                        pqxx::nontransaction select_query(db_connection);
                        pqxx::result data_from_query(select_query.exec(sql_query));
                        std::string table_name = getTableNameSelectQuery(sql_query);
                        viewTable(data_from_query, tables_map[table_name]);
                    } else {
                        pqxx::work update_db_query(db_connection);
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
            // TODO maybe throw error later;
            std::cout << "Can't open database" << std::endl;
            return 1;
        }
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}