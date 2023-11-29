#ifndef DATABASE_ERROR
#define DATABASE_ERROR

#include <stdexcept>
#include <string>

class CustomBaseError : public std::exception {
public:
    virtual const char *what() const noexcept override { return "Error. Unknown error\n"; }
};

class UnsupportedQueryError : public CustomBaseError {
public:
    virtual const char *what() const noexcept override {
        return "Error. Unsupported query. Query must starts with one of this: ACCESS, SELECT, CREATE, INSERT, DELETE, UPDATE, "
               "DROP.\n";
    }
};

class SmallQueryLengthError : public CustomBaseError {
public:
    virtual const char *what() const noexcept override {
        return "Error. Query length is less than valid query length\n";
    }
};

class InsufficientAccessRights : public CustomBaseError {
public:
    virtual const char *what() const noexcept override {
        return "Error. Insufficient access rights. You can only execute SELECT queries\n";
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

class UnopenableSocketError : public std::exception {
    virtual const char *what() const noexcept override { return "Error. Couldn't open socket\n"; }
};

class BindSocketError : public std::exception {
    virtual const char *what() const noexcept override { return "Error. Couldn't bind socket to internet address\n"; }
};

class AcceptFuncError : public std::exception {
    virtual const char *what() const noexcept override { return "Error. Couldn't execute accept function\n"; }
};

class ReadFromSocketError : public std::exception {
    virtual const char *what() const noexcept override { return "Error. Couldn't read from socket\n"; }
};

class WriteToSocketError : public std::exception {
    virtual const char *what() const noexcept override { return "Error. Couldn't write to socket\n"; }
};

class WrongHostNameError : public std::exception {
    virtual const char *what() const noexcept override { return "Error. No such host\n"; }
};

class ConnectionError : public std::exception {
    virtual const char *what() const noexcept override { return "Error. Couldn't execute connect function\n"; }
};

class LoginDataBaseError : public std::exception {
public:
    virtual const char *what() const noexcept override { return "Error. Non-existent user or password\n"; }
};

#endif
