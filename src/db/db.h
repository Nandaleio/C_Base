#ifndef DB_H
#define DB_H

#include "../libs/sqlite3.h"
#include "../libs/log.h"

// SQLite db file location
#define DB_PATH  "cb_data/data.db"
#define LOG_PATH  "cb_data/logs.log"

#define ADMIN_TABLE "cb_admin"
#define LOG_TABLE "cb_log"
#define CONFIG_TABLE "cb_config"
#define USER_TABLE "user"

typedef struct user {
  const char *name;
  const char *pass; 
  const char *token;
} user;

extern sqlite3 *db;

int db_init();
int db_close();

char *db_query(char *query);
char *db_query_param(const char* query, unsigned int count, ...);
void db_sqlite_log_callback(log_Event *ev);


#endif