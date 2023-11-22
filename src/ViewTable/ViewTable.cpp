#include "ViewTable.h"

void viewTable(const pqxx::result &res, const std::unordered_map<std::string, size_t> &column_map) {
    unsigned int rows_num = res.size();
    unsigned int cols_num = res.columns();
    for (unsigned int col_index = 0; col_index < cols_num; ++col_index) {
        const std::string str_col_name = std::string(res.column_name(col_index));
        std::cout << std::left << std::setfill(' ') << std::setw(column_map.at(str_col_name) + 1) << str_col_name;
        if (col_index != cols_num - 1) {
            std::cout << '|';
        } else {
            std::cout << '\n';
        }
    }
    for (unsigned int col_index = 0; col_index < cols_num; ++col_index) {
        const std::string str_col_name = std::string(res.column_name(col_index));
        std::cout << std::string(column_map.at(str_col_name) + 1, '-');
        if (col_index != cols_num - 1) {
            std::cout << '+';
        } else {
            std::cout << '\n';
        }
    }
    for (unsigned int row_index = 0; row_index < rows_num; ++row_index) {
        for (unsigned int col_index = 0; col_index < cols_num; ++col_index) {
            const std::string str_col_name = std::string(res.column_name(col_index));
            size_t column_size = column_map.at(str_col_name) + 1;
            const char *cstr_value = res[row_index][col_index].c_str();
            std::string str_value;
            if (strlen(cstr_value) < column_size) {
                str_value = std::string(cstr_value);
            } else {
                str_value = std::string(cstr_value, 0, column_size);
            }
            std::cout << std::left << std::setfill(' ') << std::setw(column_size) << str_value;
            if (col_index != cols_num - 1) {
                std::cout << '|';
            } else {
                std::cout << '\n';
            }
        }
    }
}
