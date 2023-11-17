#include <iostream>
#include <pqxx/pqxx>
#include <string>

template <typename T>
class MyClass {
public:
    MyClass(const T &) = delete;
};

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

        /* SELECT SQL statement */
        sql = "SELECT * from COMPANY";

        /* Create a non-transactional object. */
        pqxx::nontransaction N(C);

        /* Execute SQL query */
        pqxx::result R(N.exec(sql));

        std::cout << R.column_name(0) << '\t' << R.column_name(1) << '\t' << R.column_name(2) << '\t'
                  << R.column_name(3) << '\t' << '\t' << R.column_name(4) << '\n';
        unsigned int num_rows = R.size();
        unsigned int num_cols = R.columns();
        for (unsigned int rownum = 0; rownum < num_rows; ++rownum) {
            const pqxx::row row = R[rownum];
            for (unsigned int colnum = 0; colnum < num_cols; ++colnum) {
                const pqxx::field &field = row[colnum];
                std::cout << field.c_str() << '\t';
                /* std::cout << R[rownum][colnum].c_str() << '\t'; */
            }
            std::cout << '\n';
        }

        /* for (auto const &row : R) { */
        /*     for (auto const &field : row) */
        /*         std::cout << field.c_str() << '\t'; */
        /*     std::cout << std::endl; */
        /* } */

        /* List down all the records */
        /* for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) { */
        /*     std::cout << "ID = " << c[0].as<int>() << std::endl; */
        /*     std::cout << "Name = " << c[1].as<std::string>() << std::endl; */
        /*     std::cout << "Age = " << c[2].as<int>() << std::endl; */
        /*     std::cout << "Address = " << c[3].as<std::string>() << std::endl; */
        /*     std::cout << "Salary = " << c[4].as<float>() << std::endl; */
        /* } */
        std::cout << "Operation done successfully" << std::endl;
        C.disconnect();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
