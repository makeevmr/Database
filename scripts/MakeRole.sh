#!/bin/bash
sudo -i -u postgres psql -c "CREATE ROLE main_db WITH SUPERUSER CREATEDB CREATEROLE LOGIN ENCRYPTED PASSWORD 'Tetriandoh';"
sudo -i -u postgres createdb main_db
sudo -i -u postgres psql -c "CREATE ROLE user_db WITH SUPERUSER CREATEDB CREATEROLE LOGIN ENCRYPTED PASSWORD 'Mustache';"
sudo -i -u postgres createdb user_db
sudo -i -u postgres psql -c "\du"
sudo -i -u postgres psql -c "\l"
