#include <iostream>
#include <pqxx/pqxx>
#include <string>

int main(int argc, char *argv[]) {
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
