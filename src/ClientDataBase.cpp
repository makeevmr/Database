#include "Error.h"
#include "LoadDB/LoadDB.h"
#include <iostream>

#include <algorithm>
#include <limits>

#include <netdb.h>
#include <string.h>
#include <unistd.h>

void writeBytes(int client_sfd, char *query_buffer) {
    int writed_bytes;
    std::cin >> query_buffer;
    writed_bytes = write(client_sfd, query_buffer, strlen(query_buffer));
    if (writed_bytes < 0) {
        throw WriteToSocketError();
    }
}

bool readStaticResponse(int client_sfd, char *query_buffer, const size_t buffer_size, char *status_buffer) {
    int readed_bytes;
    readed_bytes = read(client_sfd, status_buffer, 3);
    readed_bytes = read(client_sfd, query_buffer, buffer_size - 1);
    query_buffer[readed_bytes] = '\0';
    std::cout << query_buffer;
    return status_buffer[0] == '0';
}

bool readDynamicResponse(int client_sfd, char *&query_buffer, size_t &buffer_size, char *status_buffer,
                         const size_t status_size) {
    int readed_bytes;
    int multiplier = 1;
    readed_bytes = read(client_sfd, status_buffer, status_size - 1);
    if (readed_bytes > 1) {
        int resize_count = 0;
        for (int current_index = readed_bytes - 1; current_index > 0; --current_index) {
            resize_count += (status_buffer[current_index] - '0') * multiplier;
            multiplier *= 10;
        }
        while (resize_count > 0) {
            buffer_size *= 2;
            --resize_count;
        }
        delete[] query_buffer;
        query_buffer = new char[buffer_size];
    }
    readed_bytes = read(client_sfd, query_buffer, buffer_size - 1);
    query_buffer[readed_bytes] = '\0';
    std::cout << query_buffer;
    return status_buffer[0] == '0';
}

int main() {
    try {
        std::unordered_map<std::string, std::string> server_config_map;
        std::ifstream config_json;
        loadConfig(config_json, server_config_map, 4);
        int client_sfd = 0;
        int port_num = 0;
        int writed_bytes = 0;
        struct sockaddr_in server_addr;
        struct hostent *server;
        const size_t status_size = 4;
        const size_t buffer_size = 256;
        char status_buffer[status_size];
        char query_buffer[buffer_size];
        bool is_status_ok;
        port_num = std::stoi(server_config_map["port"]);
        client_sfd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_sfd < 0) {
            throw UnopenableSocketError();
        }
        server = gethostbyname(server_config_map["IP"].c_str());
        if (server == NULL) {
            throw WrongHostNameError();
        }
        bzero(reinterpret_cast<char *>(&server_addr), sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        bcopy(reinterpret_cast<char *>(server->h_addr), reinterpret_cast<char *>(&server_addr.sin_addr.s_addr),
              server->h_length);
        server_addr.sin_port = htons(port_num);
        if (connect(client_sfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            throw ConnectionError();
        }
        std::string command;
        do {
            std::cout << "Type: 1 or 2\n1 Create a new user\n2 Login with your existing account\n";
            std::cin >> command;
        } while (command != "1" && command != "2");
        if (command == "1") {
            writed_bytes = write(client_sfd, "1", 1);
            if (writed_bytes < 0) {
                throw WriteToSocketError();
            }
            std::cout << "Enter username to sign up: ";
            writeBytes(client_sfd, query_buffer);
            std::cout << "Enter password to sign up: ";
            writeBytes(client_sfd, query_buffer);
        } else {
            writed_bytes = write(client_sfd, "2", 1);
            if (writed_bytes < 0) {
                throw WriteToSocketError();
            }
            std::cout << "Enter username to sign in: ";
            writeBytes(client_sfd, query_buffer);
            std::cout << "Enter password to sign in: ";
            writeBytes(client_sfd, query_buffer);
        }
        is_status_ok = readStaticResponse(client_sfd, query_buffer, buffer_size, status_buffer);
        if (is_status_ok) {
            is_status_ok = readStaticResponse(client_sfd, query_buffer, buffer_size, status_buffer);
        }
        std::string sql_query;
        size_t response_size = 1024;
        char *response_buffer = new char[response_size];
        while (is_status_ok && sql_query != "exit") {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << ">> ";
            std::getline(std::cin, sql_query, ';');
            writed_bytes = write(client_sfd, sql_query.c_str(), sql_query.size());
            if (writed_bytes < 0) {
                throw WriteToSocketError();
            }
            is_status_ok = readDynamicResponse(client_sfd, response_buffer, response_size, status_buffer, status_size);
        }
        delete[] response_buffer;
        close(client_sfd);
    } catch (const std::exception &e) {
        std::cout << e.what();
    }
    return 0;
}
