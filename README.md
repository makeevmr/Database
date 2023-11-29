# Database project
Database information exchange project


## Installaion
Debian-Based Linux:

1. Install the required packages. The project uses PostgreSQL database and libpqxx
API to connect databases to C++ program. You can do this by running ```sudo ./InstallPackages.sh``` from scripts folder.
Note that ``` chmod +x ./ScriptName ```may be required to run scripts.

2. Run ```./MakeDirs.sh``` from scripts folder to create obj folder and necessary files. All object files will be found in this directory and subdirectories. Note that this script should be ran withous ```sudo``` command.

3.  Run ```sudo ./MakeRoles.sh``` from scripts folder. This script will create two databases in your computer. main_db to communicate with a user and user_db for authorization process. Note that if you want, for some reason, to change databeses names or passwords in shell script you should make according changes in ```config/MainDBConfig.json``` and ```config/UserDBConfig.json``` files.
4.  Run ```make``` command from root folder of the project. This command will create three binary files. You can also ran ```make clean``` to clean all binaries and obj files.
5.  Run ```./AddFirstUser```. This binary will create table USERS in user_db database and add superuser - FirstUser with password 12345. If you want to set different username and password, you can do this by changing ```config/AddFirstUserConfig.json``` Note, that common users can only run SELECT queries.

## Supported operations
Example queries can be found in ```examples/QueryExample.txt```.

All requests must end with the character ```;``` all characters after that are ignored.

The length of the request cannot exceed 256 characters, but the length of the response can be arbitrary
