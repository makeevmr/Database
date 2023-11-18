#include <algorithm>
#include <cctype>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <unordered_map>

void skipUntil(const std::string &query, size_t &query_index, unsigned char symbol) {
    while (query[query_index] != symbol && query[query_index] != (symbol + 32)) {
        ++query_index;
    }
    ++query_index;
}

void skipSpaces(const std::string &query, size_t &query_index) {
    while (query[query_index] == ' ') {
        ++query_index;
    }
}

void pushBackColumnName(const std::string &query, std::string &column_name, bool &column_name_formed,
                        unsigned char symbol) {
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
}

void parseTableName(const std::string &query, std::string &table_name, size_t &query_index) {
    while (query[query_index] != '(' && query[query_index] != ' ') {
        if (std::isupper(query[query_index])) {
            table_name.push_back(query[query_index] + 32);
        } else {
            table_name.push_back(query[query_index]);
        }
        ++query_index;
    }
}

void parseValues(const std::string &query, const std::string &table_name, std::vector<std::string> &column_names_array,
                 std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map,
                 size_t &query_index) {
    unsigned char symbol;
    bool value_formed = false;
    bool in_single_quote = false;
    size_t column_names_index = 0;
    size_t column_names_size = column_names_array.size();
    size_t current_length = 0;
    do {
        ++query_index;
        symbol = query[query_index];
        if (!value_formed) {
            if (symbol == '\'' && !in_single_quote) {
                in_single_quote = true;
                continue;
            }
            if (in_single_quote) {
                if (symbol == '\'') {
                    in_single_quote = false;
                } else {
                    ++current_length;
                }
            } else if (symbol != ',' && symbol != ' ' && symbol != ')') {
                ++current_length;
            } else if (symbol == ' ' && current_length > 0) {
                value_formed = true;
                if (column_names_index + 2 == column_names_size) {
                    if (tables_map[table_name][column_names_array[column_names_index]] < current_length) {
                        tables_map[table_name][column_names_array[column_names_index]] = current_length;
                    }
                    ++column_names_index;
                }
            } else if (current_length > 0) {
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
            value_formed = false;
        }
    } while (column_names_index < column_names_size);
}

void parseCreateQuery(const std::string &query,
                      std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map) {
    std::string column_name;
    std::string table_name;
    size_t query_size = query.size();
    size_t query_index = 7;
    skipUntil(query, query_index, 'E');
    skipSpaces(query, query_index);
    parseTableName(query, table_name, query_index);
    query_index = query.find('(', query_index);
    bool column_name_formed = false;
    for (; query_index < query_size; ++query_index) {
        unsigned char symbol = query[query_index];
        if (!column_name_formed) {
            pushBackColumnName(query, column_name, column_name_formed, symbol);
        } else if (symbol == ',' || symbol == ';') {
            tables_map[table_name][column_name] = column_name.size();
            column_name_formed = false;
            column_name = "";
        }
    }
}

void parseInsertQuery(const std::string &query,
                      std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map) {
    std::string column_name;
    std::string table_name;
    size_t query_size = query.size();
    size_t query_index = 7;
    skipUntil(query, query_index, 'O');
    skipSpaces(query, query_index);
    parseTableName(query, table_name, query_index);
    query_index = query.find('(', query_index);
    bool column_name_formed = false;
    std::vector<std::string> column_names_array;
    unsigned char symbol;
    do {
        ++query_index;
        symbol = query[query_index];
        if (!column_name_formed) {
            pushBackColumnName(query, column_name, column_name_formed, symbol);
            if ((symbol == ',' || symbol == ')') && !column_name.empty()) {
                column_names_array.push_back(column_name);
                column_name = "";
            }
        } else if (symbol == ',' || symbol == ')') {
            column_names_array.push_back(column_name);
            column_name_formed = false;
            column_name = "";
        }
    } while (symbol != ')');
    size_t new_index = query.find('(', query_index);
    if (new_index != std::string::npos) {
        query_index = new_index;
    } else {
        query_index = query_size - 1;
    }
    while (query_index != query_size - 1) {
        parseValues(query, table_name, column_names_array, tables_map, query_index);
        new_index = query.find('(', query_index);
        if (new_index != std::string::npos) {
            query_index = new_index;
        } else {
            query_index = query_size - 1;
        }
    }
}

void parseUpdateQuery(const std::string &query,
                      std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map) {
    std::string column_name;
    std::string table_name;
    size_t query_index = 7;
    skipSpaces(query, query_index);
    parseTableName(query, table_name, query_index);
    skipUntil(query, query_index, 'T');
    while (query[query_index + 1] == ' ' || query[query_index + 1] == '(') {
        ++query_index;
    }
    bool column_name_formed = false;
    std::vector<std::string> column_names_array;
    unsigned char symbol;
    do {
        ++query_index;
        symbol = query[query_index];
        if (!column_name_formed) {
            pushBackColumnName(query, column_name, column_name_formed, symbol);
            if ((symbol == ',' || symbol == ')' || symbol == '=') && !column_name.empty()) {
                column_names_array.push_back(column_name);
                column_name = "";
            }
        } else if (symbol == ',' || symbol == ')' || symbol == '=') {
            column_names_array.push_back(column_name);
            column_name_formed = false;
            column_name = "";
        }
    } while (symbol != ')' && symbol != '=');
    while (!std::isalnum(query[query_index]) && query[query_index] != '\'') {
        ++query_index;
    }
    --query_index;
    parseValues(query, table_name, column_names_array, tables_map, query_index);
}

void parseDropQuery(const std::string &query,
                    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map) {
    std::string column_name;
    size_t query_index = 5;
    skipUntil(query, query_index, 'E');
    while (!std::isalnum(query[query_index]) && query[query_index] != '_') {
        ++query_index;
    }
    --query_index;
    bool column_name_formed = false;
    std::vector<std::string> column_names_array;
    unsigned char symbol;
    do {
        ++query_index;
        symbol = query[query_index];
        if (!column_name_formed) {
            pushBackColumnName(query, column_name, column_name_formed, symbol);
            if ((symbol == ',' || symbol == ';') && !column_name.empty()) {
                column_names_array.push_back(column_name);
                column_name = "";
            }
        } else if (symbol == ',' || symbol == ';') {
            column_names_array.push_back(column_name);
            column_name_formed = false;
            column_name = "";
        }
    } while (symbol != ';');
    for (const std::string &table_name : column_names_array) {
        tables_map.erase(table_name);
    }
}

void updateColumnSize(const std::string &query,
                      std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map) {
    std::string command = query.substr(0, 6);
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    if (command == "CREATE") {
        parseCreateQuery(query, tables_map);
    } else if (command == "INSERT") {
        parseInsertQuery(query, tables_map);
    } else if (command == "UPDATE") {
        parseUpdateQuery(query, tables_map);
    } else {
        command = query.substr(0, 4);
        std::transform(command.begin(), command.end(), command.begin(), ::toupper);
        if (command == "DROP") {
            parseDropQuery(query, tables_map);
        }
    }
}

int main(int argc, char *argv[]) {
    std::string query = "CREATE TABLE COMPANY(\
                            ID INT PRIMARY KEY     NOT NULL,\
                            NAME           TEXT    NOT NULL,\
                            AGE            INT     NOT NULL,\
                            ADDRESS        CHAR(50),\
                            SALARY         REAL);";

    /* std::string query = "CREATE TABLE COMPANY(" */
    /*                     "ID INT PRIMARY KEY     NOT NULL," */
    /*                     "NAME           TEXT    NOT NULL," */
    /*                     "AGE            INT     NOT NULL," */
    /*                     "ADDRESS        CHAR(50)," */
    /*                     "SALARY         REAL );"; */

    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> tables_map;
    updateColumnSize(query, tables_map);

    query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)\
            VALUES (1, 'Paul', 32, 'California', 20000.00 ),\
            VALUES (2, 'Allen', 25, 'Texas', 15000.00 ),\
            VALUES (3, 'Teddy', 23, 'Norway', 20000.00 ),\
            VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";
    updateColumnSize(query, tables_map);

    /* query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) " */
    /*         "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "; */
    /* updateColumnSize(query, tables_map); */
    /* query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" */
    /*         "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );"; */
    /* updateColumnSize(query, tables_map); */
    /* query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" */
    /*         "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );"; */
    /* updateColumnSize(query, tables_map); */

    query = "UPDATE COMPANY SET (id, age, name) = (1232333, 1234, 'Eugene') WHERE id = 1;";
    updateColumnSize(query, tables_map);

    query = "UPDATE COMPANY SET address = 'Californianian' WHERE id = 1;";
    updateColumnSize(query, tables_map);

    /* query = "DROP TABLE COMPANY, NewTable;"; */
    /* updateColumnSize(query, tables_map); */

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
