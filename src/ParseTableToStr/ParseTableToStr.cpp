#include "ParseTableToStr.h"

void setDelimiter(unsigned char *response, size_t &response_index, unsigned int cols_num, unsigned int col_index,
                  unsigned char delimiter) {
    if (col_index != cols_num - 1) {
        response[response_index] = delimiter;
    } else {
        response[response_index] = '\n';
    }
    ++response_index;
}

unsigned char *parseTableToStr(const pqxx::result &query_res, const std::unordered_map<std::string, size_t> &column_map,
                               unsigned char *response, size_t &response_index, size_t &response_size,
                               int &resize_count) {
    unsigned int rows_num = query_res.size();
    unsigned int cols_num = query_res.columns();
    std::vector<size_t> column_sizes_array;
    size_t total_row_size = 0;
    for (unsigned int col_index = 0; col_index < cols_num; ++col_index) {
        const char *column_name = query_res.column_name(col_index);
        size_t total_column_size = column_map.at(static_cast<std::string>(column_name)) + 3;
        column_sizes_array.push_back(total_column_size);
        total_row_size += total_column_size + 1;
        size_t current_column_size = strlen(column_name);
        if (response_size - response_index < total_column_size + 1) {
            ++resize_count;
            response_size *= 2;
            response = arrayResize<unsigned char>(response, response_index, response_size);
        }
        mempcpy(response + response_index, column_name, current_column_size);
        response_index += current_column_size;
        size_t spaces_to_add = total_column_size - current_column_size;
        memset(response + response_index, ' ', spaces_to_add);
        response_index += spaces_to_add;
        setDelimiter(response, response_index, cols_num, col_index, '|');
    }
    if (response_size - response_index < total_row_size) {
        ++resize_count;
        response_size *= 2;
        response = arrayResize<unsigned char>(response, response_index, response_size);
    }
    for (unsigned int col_index = 0; col_index < cols_num; ++col_index) {
        memset(response + response_index, '-', column_sizes_array[col_index]);
        response_index += column_sizes_array[col_index];
        setDelimiter(response, response_index, cols_num, col_index, '+');
    }
    size_t required_space = total_row_size * rows_num;
    while (response_size - response_index < required_space) {
        ++resize_count;
        response_size *= 2;
    }
    response = arrayResize<unsigned char>(response, response_index, response_size);
    for (unsigned int row_index = 0; row_index < rows_num; ++row_index) {
        for (unsigned int col_index = 0; col_index < cols_num; ++col_index) {
            const char *cstr_value = query_res[row_index][col_index].c_str();
            size_t cstr_value_len = strlen(cstr_value);
            if (cstr_value_len < column_sizes_array[col_index]) {
                memcpy(response + response_index, cstr_value, cstr_value_len);
                response_index += cstr_value_len;
                size_t spaces_to_add = column_sizes_array[col_index] - cstr_value_len;
                memset(response + response_index, ' ', spaces_to_add);
                response_index += spaces_to_add;
            } else {
                memcpy(response + response_index, cstr_value, column_sizes_array[col_index]);
                response_index += column_sizes_array[col_index];
            }
            setDelimiter(response, response_index, cols_num, col_index, '|');
        }
    }
    return response;
}
