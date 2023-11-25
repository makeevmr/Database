#ifndef DATABASE_ERROR
#define DATABASE_ERROR

#include <stdexcept>
#include <string>

class CustomBaseError : public std::exception {
public:
    virtual const char *what() const noexcept override { return "Unknown error"; }
};

class UnsupportedQueryError : public CustomBaseError {
public:
    virtual const char *what() const noexcept override {
        return "Unsupported query. Query must starts with one of this: SELECT, CREATE, INSERT, DELETE, UPDATE, DROP.";
    }
};

class SmallQueryLengthError : public CustomBaseError {
public:
    virtual const char *what() const noexcept override { return "Query length is less than valid query length"; }
};

class InsufficientAccessRights : public CustomBaseError {
public:
    virtual const char *what() const noexcept override {
        return "Insufficient access rights. You can only execute SELECT queries";
    }
};

class UnopenableFileError : public std::runtime_error {
public:
    explicit UnopenableFileError(const std::string &file_path) : std::runtime_error(file_path) {}

    virtual const char *what() const noexcept override { return std::runtime_error::what(); }
};

class UnopenableDataBaseError : public std::runtime_error {
public:
    explicit UnopenableDataBaseError(const std::string &file_path) : std::runtime_error(file_path) {}

    virtual const char *what() const noexcept override { return std::runtime_error::what(); }
};

class LoginDataBaseError : public std::exception {
public:
    virtual const char *what() const noexcept override { return "Non-existent user or password"; }
};

#endif
