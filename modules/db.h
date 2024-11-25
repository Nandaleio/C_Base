#ifndef DB_H
#define DB_H

#include <stdio.h>
#include "../libs/sqlite3.h"
#include "../libs/mongoose.h"
#include "../libs/kvec.h"

// SQLite db file location
#define DB_PATH  "cb_data/data.db"

typedef struct user {
  const char *name;
  const char *pass; 
  const char *token;
} user;

sqlite3 *db;

struct user *db_get_user(struct mg_http_message *hm);
int db_init();
int db_close();
char *db_sqlite_to_json(sqlite3_stmt *stmt);
char *db_query(char *query);
char *login(char* username, char *password);

#endif