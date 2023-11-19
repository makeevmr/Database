#include "ParseQuery/ParseQuery.h"
#include <iostream>
#include <pqxx/pqxx>


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

    /* std::unordered_map<std::string, std::unordered_map<std::string, size_t>> tables_map; */
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

    std::string sql;
    try {
        pqxx::connection C("dbname = makeevmr user = makeevmr password = Cirjkf281 hostaddr = 127.0.0.1 port = 5432");
        if (C.is_open()) {
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        } else {
            std::cout << "Can't open database" << std::endl;
            return 1;
        }

        /* CREATE SQL statement */
        sql = "CREATE TABLE COMPANY("
              "ID INT PRIMARY KEY     NOT NULL,"
              "NAME           TEXT    NOT NULL,"
              "AGE            INT     NOT NULL,"
              "ADDRESS        CHAR(50),"
              "SALARY         REAL );";

        /* INSERT SQL statement */
        /* sql = "INSERT INTO COMPANY (ID, NAME, AGE, ADDRESS, SALARY) " */
        /*       "VALUES (5, 'Paul', 32, 'California', 20000.00 ), " */
        /*       "(6, 'Allen', 25, 'Texas', 15000.00 )," */
        /*       "(7, 'Teddy', 23, 'Norway', 20000.00 )," */
        /*       "(8, 'Mark', 25, 'Rich-Mond ', 65000.00 );"; */

        /* Create a transactional object. */
        pqxx::work W(C);

        /* Execute SQL query */

        W.exec(sql);
        W.commit();
        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
