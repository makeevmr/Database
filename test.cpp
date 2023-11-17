#include <algorithm>
#include <cctype>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <unordered_map>

void updateColumnSize(const std::string &query,
                      std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map) {
    std::string column_name;
    std::string table_name;
    std::string command = query.substr(0, 6);
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    size_t query_size = query.size();
    size_t query_index;
    if (command == "CREATE") {
        query_index = std::min(query.find('e', 7), query.find('E', 7)) + 1;
        while (query[query_index] == ' ') {
            ++query_index;
        }
        while (query[query_index] != '(' && query[query_index] != ' ') {
            if (std::isupper(query[query_index])) {
                table_name.push_back(query[query_index] + 32);
            } else {
                table_name.push_back(query[query_index]);
            }
            ++query_index;
        }
        bool in_parenthesis = false;
        bool column_name_formed = false;
        for (; query_index < query_size; ++query_index) {
            unsigned char symbol = query[query_index];
            if (!in_parenthesis) {
                if (symbol == '(') {
                    in_parenthesis = true;
                }
                continue;
            } else {
                if (!column_name_formed) {
                    if (std::isalnum(symbol) || symbol == '_') {
                        if (std::isupper(symbol)) {
                            column_name.push_back(symbol + 32);
                        } else {
                            column_name.push_back(symbol);
                        }
                    }
                    if (symbol == ' ' && !column_name.empty()) {
                        column_name_formed = true;
                    }
                } else if (symbol == ',' || symbol == ';') {
                    tables_map[table_name][column_name] = column_name.size();
                    column_name_formed = false;
                    column_name = "";
                }
            }
        }
    }
    if (command == "INSERT") {
        query_index = std::min(query.find('o', 7), query.find('O', 7)) + 1;
        while (query[query_index] == ' ') {
            ++query_index;
        }
        while (query[query_index] != '(' && query[query_index] != ' ') {
            if (std::isupper(query[query_index])) {
                table_name.push_back(query[query_index] + 32);
            } else {
                table_name.push_back(query[query_index]);
            }
            ++query_index;
        }
        bool in_parenthesis = false;
        bool column_name_formed = false;
        std::vector<std::string> column_names_array;
        unsigned char symbol;
        do {
            ++query_index;
            symbol = query[query_index];
            // TODO delete below if statement
            if (!in_parenthesis) {
                if (symbol == '(') {
                    in_parenthesis = true;
                }
                continue;
            } else {
                if (!column_name_formed) {
                    if (std::isalnum(symbol) || symbol == '_') {
                        if (std::isupper(symbol)) {
                            column_name.push_back(symbol + 32);
                        } else {
                            column_name.push_back(symbol);
                        }
                    }
                    if (symbol == ' ' && !column_name.empty()) {
                        column_name_formed = true;
                    }
                    if ((symbol == ',' || symbol == ')') && !column_name.empty()) {
                        column_names_array.push_back(column_name);
                        column_name = "";
                    }
                } else if (symbol == ',' || symbol == ')') {
                    column_names_array.push_back(column_name);
                    column_name_formed = false;
                    column_name = "";
                }
            }
        } while (symbol != ')');
        query_index = query.find('(', query_index);
        column_name_formed = false;
        bool in_single_quote = false;
        size_t current_length = 0;
        int column_names_index = 0;
        do {
            ++query_index;
            symbol = query[query_index];
            if (!column_name_formed) {
                if (symbol == '\'' && !in_single_quote) {
                    in_single_quote = true;
                    continue;
                }
                if (!in_single_quote && symbol != ',' && symbol != ' ' && symbol != ')') {
                    ++current_length;
                }
                if (in_single_quote) {
                    if (symbol == '\'') {
                        in_single_quote = false;
                    } else {
                        ++current_length;
                    }
                }
                if (!in_single_quote && symbol == ' ' && current_length > 0) {
                    column_name_formed = true;
                }
                if ((symbol == ',' || symbol == ')') && current_length > 0) {
                    if (tables_map[table_name][column_names_array[column_names_index]] < current_length) {
                        tables_map[table_name][column_names_array[column_names_index]] = current_length;
                    }
                    ++column_names_index;
                    current_length = 0;
                }
            } else if (symbol == ',' || symbol == ')') {
                if (tables_map[table_name][column_names_array[column_names_index]] < current_length) {
                    tables_map[table_name][column_names_array[column_names_index]] = current_length;
                }
                ++column_names_index;
                current_length = 0;
                column_name_formed = false;
            }
        } while (symbol != ')');
    }
}

int main(int argc, char *argv[]) {
    /* std::string query = "cReaTe   TAbLe   COMPANY(\ */
    /*                     ID INT PRIMARY KEY     NOT NULL,\ */
    /*                     NAME           TEXT    NOT NULL,\ */
    /*                     AGE            INT     NOT NULL,\ */
    /*                     ADDRESS        CHAR(50),\ */
    /*                     SALARY         REAL );"; */
    /*  */
    std::string query = "CREATE TABLE COMPANY("
                        "ID INT PRIMARY KEY     NOT NULL,"
                        "NAME           TEXT    NOT NULL,"
                        "AGE            INT     NOT NULL,"
                        "ADDRESS        CHAR(50),"
                        "SALARY         REAL );";

    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> tables_map;
    updateColumnSize(query, tables_map);

    query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "
            "VALUES (1, 'Paul', 32, 'California', 20000.00 ); ";
    updateColumnSize(query, tables_map);
    query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "
            "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); ";
    updateColumnSize(query, tables_map);
    query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)"
            "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );";
    updateColumnSize(query, tables_map);
    query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)"
            "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";
    updateColumnSize(query, tables_map);

    for (auto b_iter1 = tables_map.begin(); b_iter1 != tables_map.end(); ++b_iter1) {
        std::cout << "Table: " << b_iter1->first << '\n';
        for (auto b_iter2 = b_iter1->second.begin(); b_iter2 != b_iter1->second.end(); ++b_iter2) {
            std::cout << b_iter2->first << ' ' << b_iter2->second << '\n';
        }
    }
    /* std::string sql; */
    /* std::unordered_map<std::string, std::unordered_map<std::string, unsigned int>> tables_map; */
    /*  */
    /* try { */
    /*     pqxx::connection C("dbname = makeevmr user = makeevmr password = Cirjkf281 hostaddr = 127.0.0.1 port =
     * 5432"); */
    /*     if (C.is_open()) { */
    /*         std::cout << "Opened database successfully: " << C.dbname() << std::endl; */
    /*     } else { */
    /*         std::cout << "Can't open database" << std::endl; */
    /*         return 1; */
    /*     } */

    /* CREATE SQL statement */
    /* sql = "CREATE TABLE COMPANY(" */
    /*       "ID INT PRIMARY KEY     NOT NULL," */
    /*       "NAME           TEXT    NOT NULL," */
    /*       "AGE            INT     NOT NULL," */
    /*       "ADDRESS        CHAR(50)," */
    /*       "SALARY         REAL );"; */

    /* INSERT SQL statement */
    /* sql = "INSERT INTO COMPANY (ID, NAME, AGE, ADDRESS, SALARY) " */
    /*       "VALUES (5, 'Paul', 32, 'California', 20000.00 ), " */
    /*       "(6, 'Allen', 25, 'Texas', 15000.00 )," */
    /*       "(7, 'Teddy', 23, 'Norway', 20000.00 )," */
    /*       "(8, 'Mark', 25, 'Rich-Mond ', 65000.00 );"; */

    /* Create a transactional object. */
    /* pqxx::work W(C); */

    /* Execute SQL query */
    /*     W.exec(sql); */
    /*  */
    /*     W.commit(); */
    /*     C.disconnect(); */
    /* } catch (const std::exception &e) { */
    /*     std::cerr << e.what() << std::endl; */
    /*     return 1; */
    /* } */
    return 0;
}
