#ifndef DATABASE_ERROR
#define DATABASE_ERROR

#include <exception>
#include <iostream>

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

class SmallQueryLength : public CustomBaseError {
public:
    virtual const char *what() const noexcept override { return "Query length is less than valid query length"; }
};

#endif
