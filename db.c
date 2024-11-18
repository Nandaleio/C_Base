

#include "db.h"

struct user {
  const char *name;
  const char *pass; 
  const char *token;
};

// Parse HTTP requests, return authenticated user or NULL
static struct user *getuser(struct mg_http_message *hm) {
    //TODO use SQLite DB
    static struct user users[] = {
        {"admin", "admin", "admin_token"},
        {"user1", "pass1", "user1_token"},
        {"user2", "pass2", "user2_token"},
        {NULL, NULL, NULL},
    };

    char user[256], pass[256];
    struct user *u;
    mg_http_creds(hm, user, sizeof(user), pass, sizeof(pass));
    printf("Credentials: %s: %s\r\n", user, pass);
    if (user[0] == '\0') { // token auth
        for (u = users; u->name != NULL; u++)
        if (strcmp(pass, u->token) == 0) {
            return u;
        }
    } else if (user[0] != '\0' && pass[0] != '\0') { // login user & pass
        for (u = users; u->name != NULL; u++)
        if (strcmp(user, u->name) == 0 && strcmp(pass, u->pass) == 0) {
            return u;
        }
    }
    return NULL;
}

int init_db() {
    
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    sqlite3_stmt* stmt = NULL;

    int retval;
    char *sql = "CREATE TABLE IF NOT EXISTS admin ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created TEXT DEFAULT (SELECT DATE()),"
        "email TEXT NOT NULL, "
        "password TEXT NOT NULL "
    ")";
    retval = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if(retval != SQLITE_OK) {
        printf("%s", sqlite3_errstr(retval));
        return 1;
    }
    retval = sqlite3_step(stmt);
    if(retval != SQLITE_DONE) {
        printf("%s", sqlite3_errstr(retval));
    }
    retval = sqlite3_finalize(stmt);

    //create_tables_if_not_existing("admin");
    //create_tables_if_not_existing("users");

    return 0;
}

int create_tables_if_not_existing(const char *table_name) {
    sqlite3_stmt* stmt = NULL;

    char *sql = "CREATE TABLE IF NOT EXISTS admin ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created TEXT DEFAULT (SELECT DATE()),"
        "email TEXT NOT NULL, "
        "password TEXT NOT NULL "
    ")";

    int retval = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if(retval != SQLITE_OK) {
        printf("%s", sqlite3_errstr(retval));
        return 1;
    }

    // retval = sqlite3_bind_text(stmt, 1, table_name, sizeof(table_name), NULL);
    // if (retval != SQLITE_OK) {
    //     printf("%s", sqlite3_errstr(retval));
    //     return 1;
    // }

    retval = sqlite3_step(stmt);
    if(retval != SQLITE_DONE) {
        printf("%s", sqlite3_errstr(retval));
    }
    retval = sqlite3_finalize(stmt);
    return retval;
}

int close_db() {
    return sqlite3_close(db);
}