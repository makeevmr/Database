#include "JsonHandle.h"

inline void writeIndent(std::ofstream &output_json, unsigned short int layers) {
    for (unsigned short int current_layer = 0; current_layer < layers; ++current_layer) {
        output_json << '\t';
    }
}

void writeTablesMapToJson(std::ofstream &output_json,
                          const std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map) {
    output_json << "{\n";
    bool first_table = true;
    unsigned short int layers = 0;
    auto e_table_iter = tables_map.end();
    for (auto b_table_iter = tables_map.begin(); b_table_iter != e_table_iter; ++b_table_iter) {
        if (!first_table) {
            output_json << ",\n";
        }
        layers = 1;
        writeIndent(output_json, layers);
        output_json << "\"" + b_table_iter->first + "\": {\n";
        bool first_column = true;
        auto e_column_iter = b_table_iter->second.end();
        for (auto b_column_iter = b_table_iter->second.begin(); b_column_iter != e_column_iter; ++b_column_iter) {
            if (!first_column) {
                output_json << ",\n";
            }
            layers = 2;
            writeIndent(output_json, layers);
            output_json << "\"" + b_column_iter->first + "\": " << b_column_iter->second;
            first_column = false;
        }
        layers = 1;
        output_json << '\n';
        writeIndent(output_json, layers);
        output_json << "}";
        first_table = false;
    }
    output_json << "\n}";
}

void readTablesMapFromJson(std::ifstream &input_json,
                           std::unordered_map<std::string, std::unordered_map<std::string, size_t>> &tables_map) {
    std::string table_name;
    std::string column_name;
    std::string value;
    bool is_table_set = false;
    bool is_column_set = false;
    unsigned char current_symbol = 0;
    while (input_json.peek() != EOF) {
        current_symbol = static_cast<unsigned char>(input_json.get());
        while (!is_table_set) {
            while (current_symbol != '"' && input_json.peek() != EOF) {
                current_symbol = static_cast<unsigned char>(input_json.get());
            }
            if (current_symbol == '"') {
                current_symbol = static_cast<unsigned char>(input_json.get());
                while (current_symbol != '"') {
                    table_name.push_back(current_symbol);
                    current_symbol = static_cast<unsigned char>(input_json.get());
                }
                is_table_set = true;
            } else {
                break;
            }
        }
        if (!is_table_set) {
            break;
        } else {
            while (current_symbol != '}') {
                current_symbol = static_cast<unsigned char>(input_json.get());
                while (!is_column_set) {
                    while (current_symbol != '"' && current_symbol != '}') {
                        current_symbol = static_cast<unsigned char>(input_json.get());
                    }
                    if (current_symbol == '"') {
                        current_symbol = static_cast<unsigned char>(input_json.get());
                        while (current_symbol != '"') {
                            column_name.push_back(current_symbol);
                            current_symbol = static_cast<unsigned char>(input_json.get());
                        }
                        is_column_set = true;
                        while (current_symbol != ',' && current_symbol != '}') {
                            current_symbol = static_cast<unsigned char>(input_json.get());
                            if (47 < current_symbol && current_symbol < 58) {
                                value.push_back(current_symbol);
                            }
                        }
                        tables_map[table_name][column_name] = std::stoull(value);
                        is_column_set = false;
                        column_name = "";
                        value = "";
                    } else {
                        break;
                    }
                }
                if (!is_column_set) {
                    is_table_set = false;
                    table_name = "";
                    break;
                }
            }
        }
    }
}
