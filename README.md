# Database project
A project to execute SQL queries to the database over a local network.


## Installaion
Debian-Based Linux:

1. Install the required packages. The project uses PostgreSQL database and libpqxx
API to connect databases to C++ program. You can do this by running ```sudo ./InstallPackages.sh``` from scripts folder.
Note that ``` chmod +x ./ScriptName ```may be required to run scripts.

2. Run ```./MakeDirs.sh``` from scripts folder to create obj folder and necessary files. All object files will be found in this directory and subdirectories. Note that this script should be ran withous ```sudo``` command.

3.  Run ```sudo ./MakeRoles.sh``` from scripts folder. This script will create two databases in your computer. main_db to communicate with a user and user_db for authorization process. Note that if you want, for some reason, to change databases names or passwords in shell script you should make according changes in ```config/MainDBConfig.json``` and ```config/UserDBConfig.json``` files.
4.  Run ```make``` command from root folder of the project. This command will create three binary files. You can also run ```make clean``` to clean all binaries and obj files.
5.  Run ```./AddFirstUser```. This binary will create table USERS in user_db database and add superuser - FirstUser with password 12345. If you want to set different username and password, you can do this by changing ```config/AddFirstUserConfig.json``` Note, that common users can only run SELECT queries.

## General description of commands
Example queries can be found in ```examples/QueryExample.txt```.

All requests must end with the character ```;``` all characters after that are ignored.

The length of the request cannot exceed 256 characters, but the length of the response can be arbitrary

SQL keywords, names for tables and columns are not case sensitive (except for the request ```exit;```), so commands like ```SELECT ID```, ```select id``` and ```SelECt Id``` will give the same results,  however keep in mind that values in database ake case sensetive.

The request can contain an arbitrary number of spaces, tabs and newlines (if the request itself is less than 256 characters)
so request like so also will work:
```
CREate TabLE   COmpANY 
     (
            id   INT PRimaRY KEY     NOT NULL,
    NamE             TexT    Not nuLL,
         
         Age            inT     NOT NULL,AdDRESS        CHAR(50),
        SAlary         reaL);

```

## Supported commands
1. ```exit;``` to end the session. This command will be accepted as a session exit only in lowercase.

2. ```ACCESS username1;``` or ```ACCESS username1, username2, username3;``` to give one or more users superuser rights to change the state of the database. Naturally, the user executing the command must be a superuser himself.

3.  ```SELECT``` to display a table or part of it. Examples:
    ```
    SELECT * FROM COMPANY;
    SELECT id, name FROM COMPANY;
    SELECT id, name From company WHERE id = 2;
    ```

4. ```CREATE``` to create database. Examples:
    ```
    CREATE TABLE COMPANY(
        ID INT PRIMARY KEY     NOT NULL,
        NAME           TEXT    NOT NULL,
        AGE            INT     NOT NULL,
        ADDRESS        CHAR(50),
        SALARY         REAL);
    ```

    ```
    CREATE TABLE USERS(
        ID SERIAL PRIMARY KEY,
        USERNAME CHAR(50),
        PASSWORD CHAR(50)  NOT NULL,
        IS_SUPERUSER  BOOLEAN NOT NULL
    );
    ```
5. ```INSERT``` to insert new rows into table. Examples:
    ```
    INSERT INTO COMPANY (ID, NAME, AGE, ADDRESS, SALARY) VALUES
    (1, 'Paul', 32, 'California', 20000.00 );
    
    ```

    ```
    INSERT INTO COMPANY (ID, NAME, AGE, ADDRESS, SALARY) VALUES
    (2, 'Allen', 25, 'Texas', 15000.00 ),
    (3, 'Teddy', 23, 'Norway', 20000.00 ),
    (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );
    ```
6. ```DELETE``` to delete a record from the database. Examples:
    ```
    DELETE FROM COMPANY WHERE id = 3;
    ```
    to delete all records from table:

    ```
    DELETE FROM company;
    ```
7. ```UPDATE``` to update an existing record. Examples:


    ```
    UPDATE COMPANY SET (id, age, name) = (12, 34, 'Eugene') WHERE id = 1;
    UPDATE COMPANY SET age = 40 WHERE id = 2;
    UPDATE COMPANY SET age = 40 WHERE age = 25;
    
    ```
    
8. ```DROP``` to delete a table or multiple tables. Examples:

    ```
    DROP TABLE company;
    DROP TABLE company, accounts;
    ```
