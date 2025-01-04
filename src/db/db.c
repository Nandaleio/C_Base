

#include <stdio.h>

#include "../libs/parson.h"

#include "db.h"

#include "../utils/utils.h"
#include "../utils/jwt.h"
#include "../utils/info.h"

sqlite3 *db;

int db_init() {
    
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        log_error("Failed to open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    sqlite3_stmt* stmt = NULL;

    int retval;
    char *errMsg = 0;

    char *sql = "CREATE TABLE IF NOT EXISTS " ADMIN_TABLE " ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created INTEGER DEFAULT (unixepoch('now')),"
        "username TEXT UNIQUE NOT NULL, "
        "email TEXT, "
        "avatar BLOB, "
        "password TEXT NOT NULL, "
        "salt TEXT NOT NULL)";

    // Execute the SQL statement
    retval = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (retval != SQLITE_OK) {
        log_error("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
    } else {
        log_trace("Table \""ADMIN_TABLE"\" created successfully.");
    }

    // -- ADD DEFAULT ADMIN --
    sql = "INSERT INTO " ADMIN_TABLE "(username, password, salt, email)"
        "SELECT 'admin', ?, ?, 'admin@C_Base.com'"
        "WHERE NOT EXISTS (SELECT 1 FROM "ADMIN_TABLE")";

    char salt[SALT_LENGTH+1];
    char *admin_pass = hash_password("admin", salt);

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        free(admin_pass);
    }

    if (sqlite3_bind_text(stmt, 1, admin_pass, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind admin password parameter: %s", sqlite3_errmsg(db));
        free(admin_pass);
    }
    if (sqlite3_bind_text(stmt, 2, salt, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind admin salt parameter: %s", sqlite3_errmsg(db));
        free(admin_pass);
    }
    
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        log_error("Failed to insert user: %s", sqlite3_errmsg(db));
        free(admin_pass); 
	}
    
	if(sqlite3_finalize(stmt) != SQLITE_OK){
        log_error("Failed finalize statement: %s", sqlite3_errmsg(db));
        free(admin_pass); 
    }
    
    free(admin_pass); 
    // -- END ADD DEFAULT ADMIN --

    sql = "CREATE TABLE IF NOT EXISTS " USER_TABLE " ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created INTEGER DEFAULT (unixepoch('now')),"
        "username TEXT UNIQUE NOT NULL, "
        "email TEXT, "
        "avatar BLOB, "
        "password TEXT NOT NULL, "
        "salt TEXT NOT NULL)";
    retval = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (retval != SQLITE_OK) {
        log_error("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
    } else {
        log_trace("Table \""USER_TABLE"\" created successfully.");
    }

    sql = "CREATE TABLE IF NOT EXISTS " LOG_TABLE " ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created INTEGER DEFAULT (unixepoch('now')),"
        "level TEXT NOT NULL, "
        "description TEXT NOT NULL)";
    retval = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (retval != SQLITE_OK) {
        log_error("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
    } else {
        log_trace("Table \""LOG_TABLE"\" created successfully.");
    }

    sql = "CREATE TABLE IF NOT EXISTS " CONFIG_TABLE " ("
        "name TEXT PRIMARY KEY, value TEXT, type TEXT)";
    retval = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (retval != SQLITE_OK) {
        log_error("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
    } else {
        log_trace("Table \""CONFIG_TABLE"\" created successfully.");
    }

    sql = "INSERT INTO " CONFIG_TABLE " VALUES "
    "('ALLOW REGISTER', 'true', 'BOOLEAN'), "
    "('JWT DURATION', '3600', 'NUMBER') ";

    int no_ret = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (no_ret != SQLITE_OK) {
        log_error("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
    } else {
        log_trace("Table \""CONFIG_TABLE"\" created successfully.");
    }

    return retval;
}

int db_close() {
    return sqlite3_close(db);
}


char * db_query_param(const char* query, unsigned int count, ...) {
    
    log_debug("received query param : %s", query);

    va_list args;
    va_start(args, query);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to prepare statement\"}";
    }
    for(int i = 1 ; i <= count; i++){
        char* param = va_arg(args, char*);
        if (sqlite3_bind_text(stmt, i, param, -1, SQLITE_STATIC) != SQLITE_OK) {
            log_error("Failed to bind parameter: %s", sqlite3_errmsg(db));
            return "{\"error\": \"Failed to bind parameter\"}";
        }
    }
    va_end(args);

    char *json = db_sqlite_to_json(stmt);

    if (!json) {
        log_error("Failed to convert result set to JSON");
    }

    if(sqlite3_finalize(stmt) != SQLITE_OK) {
        log_error("Failed to finalize the statement");
    }

    return json;
}

char *db_query(char* query) {

    log_debug("received query : %s", query);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to prepare statement\"}";
    }

    char *json = db_sqlite_to_json(stmt);

    if (!json) {
        log_error("Failed to convert result set to JSON");
    }

    if(sqlite3_finalize(stmt) != SQLITE_OK) {
        log_error("Failed to finalize the statement");
    }

    return json;
}

void db_sqlite_log_callback(log_Event *ev) {
    const char *sql = "INSERT INTO " LOG_TABLE " (level, description) VALUES (?, ?);";
    sqlite3_stmt *stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        log_error(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    int size = vsnprintf(NULL, 0, ev->fmt, ev->ap);
    char *final_message = (char*)malloc(size + 1);
    vsnprintf(final_message, size + 1, ev->fmt, ev->ap);
    // Bind log event parameters to the SQL statement
    sqlite3_bind_text(stmt, 1, log_level_string(ev->level), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, final_message, -1, SQLITE_STATIC);

    // Execute the SQL statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        log_error(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    free(final_message);
}




