#ifndef DB_H
#define DB_H

#include <stdio.h>
#include "libs/sqlite3.h"

// SQLite db file location
#define DB_PATH  "cb_data/data.db"

typedef struct user user;

sqlite3 *db;

struct user *getuser(struct mg_http_message);
int init_db();
int create_tables_if_not_existing();
int close_db();

#endif