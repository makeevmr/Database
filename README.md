# Database project
Database information exchange project


## Installaion
Debian-Based Linux:
1. Install PostgreSQL:
    ```
    sudo apt-get update -y
    sudo apt install postgresql postgresql-contrib
    ```

2. Install libpqxx API to be able to make requests to PostgreSQL database from C++ program:
    ```
    sudo apt-get update -y
    sudo apt-get install -y libpqxx-dev
    sudo apt-get install -y nlohmann-json3-dev
    ```
