#ifndef DB_H
#define DB_H

#include "../libs/sqlite3.h"

// SQLite db file location
#define DB_PATH  "cb_data/data.db"
#define LOG_PATH  "cb_data/logs.log"

#define ADMIN_TABLE "cb_admin"
#define LOG_TABLE "cb_log"
#define USER_TABLE "user"

typedef struct user {
  const char *name;
  const char *pass; 
  const char *token;
} user;

extern sqlite3 *db;

int db_init();
int db_close();

// UTILS
char *db_sqlite_to_json(sqlite3_stmt *stmt);
char *db_query(char *query);

//TABLES
char *db_get_tables();
char *db_get_table(char *table_name);

//AUTH
char *db_add_user(char *username, char *password);
char *db_login(char* username, char *password);
char *db_admin_login(char *username, char *password);

#endif