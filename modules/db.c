

#include "db.h"
#include "../libs/log.h"
#include <stdio.h>
#include "utils.h"
#include "jwt.h"

// Parse HTTP requests, return authenticated user or NULL
struct user *db_get_user(struct mg_http_message *hm) {

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

    log_debug("Credentials: %s: %s\r", user, pass);
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

int db_init() {
    
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        log_error("Failed to open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    sqlite3_stmt* stmt = NULL;

    int retval;
    char *errMsg = 0;

    char *sql = "CREATE TABLE IF NOT EXISTS admin ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "email TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL "
    ")";

    // Execute the SQL statement
    retval = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (retval != SQLITE_OK) {
        log_error("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
    } else {
        log_trace("Table \"admin\" created successfully.");
    }

    sql = "CREATE TABLE IF NOT EXISTS user ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created TEXT DEFAULT (CURRENT_TIMESTAMP),"
        "username TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL, "
        "salt TEXT NOT NULL "
    ")";
    retval = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (retval != SQLITE_OK) {
        log_error("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
    } else {
        log_trace("Table \"user\" created successfully.");
    }

    sql = "CREATE TABLE IF NOT EXISTS log ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created TEXT DEFAULT (CURRENT_TIMESTAMP),"
        "level TEXT NOT NULL, "
        "description TEXT NOT NULL "
    ")";
    retval = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (retval != SQLITE_OK) {
        log_error("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
    } else {
        log_trace("Table \"log\" created successfully.");
    }

    return retval;
}

int db_close() {
    return sqlite3_close(db);
}

char *db_query(char* query) {


    log_debug("received query : %s \r", query);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to prepare statement\"}";
    }

    char *json = db_sqlite_to_json(stmt);

    if (json) {
        log_debug("JSON Result:%s", json);
    } else {
        log_error("Failed to convert result set to JSON");
    }

    if(sqlite3_finalize(stmt) != SQLITE_OK) {
        log_error("Failed to finalize the statement");
    }

    return json;
}

char *db_get_tables() {
    char *query = "SELECT name FROM sqlite_master WHERE type='table' and name not like 'sqlite_%' and name not in ('admin', 'log')";
    char *json = db_query(query);
    return json;
}

char *db_add_user(char *username, char *password) {
    
    log_info("Adding new user: %s", username);
    char* query = "INSERT INTO user (username, salt, password) VALUES (?,?,?)";

    log_debug("%s", query);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to prepare statement\"}";
    }

    if (sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind username parameter: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to bind username parameter\"}";
    }

    char salt[SALT_LENGTH];
    char *hash_pass = hash_password(password, salt);

    if (sqlite3_bind_text(stmt, 2, salt, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind username parameter: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to bind username parameter\"}";
    }

    if (sqlite3_bind_text(stmt, 3, hash_pass, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind password parameter: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to bind password parameter\"}";
    }

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        log_error("Failed to insert user: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to insert user\"}";
	}

    
	if(sqlite3_finalize(stmt) != SQLITE_OK){
        log_error("Failed finalize statement: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed finalize statement:\"}";
    }

    return "{\"error\": \"\"}";
}

char *db_login(char *username, char *password) {
    
    log_trace("User %s logging in", username);
    char *query = "select password, salt from user where username = ?";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to prepare statement\"}";
    }
    
    if (sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind username parameter: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to bind username parameter\"}";
    }
    
    log_trace("qsdqsdqsdqsdqsd row ?");

    if(sqlite3_step(stmt) != SQLITE_ROW) {
        log_error("Failed to insert user: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to insert user\"}";
	}

    
    log_trace("IS row ?");

    char* hashed_password = sqlite3_column_text(stmt, 1);
    char* salt = sqlite3_column_text(stmt, 2);

    if(check_password(hashed_password, password, salt) != 0) {
        log_error("The passwords doesn't match");
        return "{\"error\": \"wrong password\"}";
    }

    const char *header = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";
    const char *payload = "{\"sub\":\"1234567890\",\"name\":\"John Doe\",\"iat\":1516239022}";
    return jwt_sign(header, payload, JWT_SECRET_KEY);
}
