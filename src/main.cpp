#include "ParseQuery/ParseQuery.h"
#include "ViewTable/ViewTable.h"

int main(int argc, char *argv[]) {
    /* std::string query = "CREATE TABLE COMPANY(\ */
    /*                         ID INT PRIMARY KEY     NOT NULL,\ */
    /*                         NAME           TEXT    NOT NULL,\ */
    /*                         AGE            INT     NOT NULL,\ */
    /*                         ADDRESS        CHAR(50),\ */
    /*                         SALARY         REAL);"; */

    /* std::string query = "CREATE TABLE COMPANY(" */
    /*                     "ID INT PRIMARY KEY     NOT NULL," */
    /*                     "NAME           TEXT    NOT NULL," */
    /*                     "AGE            INT     NOT NULL," */
    /*                     "ADDRESS        CHAR(50)," */
    /*                     "SALARY         REAL );"; */

    /* updateColumnSize(query, tables_map); */
    /*  */
    /* query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)\ */
    /*         VALUES (1, 'Paul', 32, 'California', 20000.00 ),\ */
    /*         VALUES (2, 'Allen', 25, 'Texas', 15000.00 ),\ */
    /*         VALUES (3, 'Teddy', 23, 'Norway', 20000.00 ),\ */
    /*         VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );"; */
    /* updateColumnSize(query, tables_map); */

    /* query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) " */
    /*         "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "; */
    /* updateColumnSize(query, tables_map); */
    /* query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" */
    /*         "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );"; */
    /* updateColumnSize(query, tables_map); */
    /* query = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" */
    /*         "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );"; */
    /* updateColumnSize(query, tables_map); */

    /* query = "UPDATE COMPANY SET (id, age, name) = (1232333, 1234, 'Eugene') WHERE id = 1;"; */
    /* updateColumnSize(query, tables_map); */

    /* query = "UPDATE COMPANY SET address = 'Californianian' WHERE id = 1;"; */
    /* updateColumnSize(query, tables_map); */

    /* query = "DROP TABLE COMPANY, NewTable;"; */
    /* updateColumnSize(query, tables_map); */

    /* for (auto b_iter1 = tables_map.begin(); b_iter1 != tables_map.end(); ++b_iter1) { */
    /*     std::cout << "Table: " << b_iter1->first << '\n'; */
    /*     for (auto b_iter2 = b_iter1->second.begin(); b_iter2 != b_iter1->second.end(); ++b_iter2) { */
    /*         std::cout << b_iter2->first << ' ' << b_iter2->second << '\n'; */
    /*     } */
    /* } */

    std::string sql_query;
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> tables_map;
    try {
        pqxx::connection db_connection(
            "dbname = makeevmr user = makeevmr password = Cirjkf281 hostaddr = 127.0.0.1 port = 5432");
        if (db_connection.is_open()) {
            std::cout << "Opened database successfully: " << db_connection.dbname() << std::endl;
            while (true) {
                std::cout << ">> ";
                std::getline(std::cin, sql_query);
                if (sql_query == "exit") {
                    db_connection.disconnect();
                    break;
                }
                /* "create, insert, update, select, drop"; */
                if (!sql_query.empty() && (sql_query[0] == 'S' || sql_query[0] == 's')) {
                    pqxx::nontransaction select_query(db_connection);
                    pqxx::result data_from_query(select_query.exec(sql_query));
                    unsigned int oid = data_from_query.column_table(0);
                    std::string get_table_name = "SELECT relname FROM pg_class WHERE oid = " + std::to_string(oid);
                    std::string table_name = std::string(
                        pqxx::result(pqxx::nontransaction(db_connection).exec(get_table_name))[0][0].c_str());
                    viewTable(data_from_query, tables_map[table_name]);
                    /* viewTable(data_from_query, tables_map["company"]); */
                } else if (!sql_query.empty()) {
                    pqxx::work update_db_query(db_connection);
                    update_db_query.exec(sql_query);
                    update_db_query.commit();
                    updateColumnSize(sql_query, tables_map);
                } else {
                    std::cout << "Empty SQL query";
                    // TODO throw error here on empty sql_query
                }
            }
        } else {
            std::cout << "Can't open database" << std::endl;
            return 1;
        }

        /* CREATE SQL statement */
        /* sql_query = "CREATE TABLE COMPANY(" */
        /*             "ID INT PRIMARY KEY     NOT NULL," */
        /*             "NAME           TEXT    NOT NULL," */
        /*             "AGE            INT     NOT NULL," */
        /*             "ADDRESS        CHAR(50)," */
        /*             "SALARY         REAL );"; */

        /* INSERT SQL statement */
        /* sql = "INSERT INTO COMPANY (ID, NAME, AGE, ADDRESS, SALARY) " */
        /*       "VALUES (5, 'Paul', 32, 'California', 20000.00 ), " */
        /*       "(6, 'Allen', 25, 'Texas', 15000.00 )," */
        /*       "(7, 'Teddy', 23, 'Norway', 20000.00 )," */
        /*       "(8, 'Mark', 25, 'Rich-Mond ', 65000.00 );"; */

    } catch (const std::exception &e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
