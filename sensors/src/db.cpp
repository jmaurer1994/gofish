#include "db.h"
AsyncClient client;
IPAddress dbaddr(DB_ADDRESS);
PGconnection db(&client);

bool initialize_database(){
    db.setDbLogin(dbaddr, DB_USER, DB_PASSWORD,DB_DATABASE, NULL, DB_PORT);
}