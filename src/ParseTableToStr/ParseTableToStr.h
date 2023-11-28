#ifndef DATABASE_VIEW_TABLE
#define DATABASE_VIEW_TABLE

#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <unordered_map>

template <typename T>
T *arrayResize(T *array, size_t array_len, size_t &array_size) {
    array_size *= 2;
    T *new_array = new T[array_size];
    memcpy(new_array, array, array_len);
    delete[] array;
    array = new_array;
    return array;
}

unsigned char *parseTableToStr(const pqxx::result &query_res, const std::unordered_map<std::string, size_t> &column_map,
                               unsigned char *response, size_t &response_index, size_t &response_size);

#endif // DATABASE_VIEW_TABLE
