

#include <stdio.h>

#include "../libs/parson.h"

#include "../includes/db.h"
#include "../includes/utils.h"
#include "../includes/jwt.h"

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
        "created TEXT DEFAULT (CURRENT_TIMESTAMP),"
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
        "created TEXT DEFAULT (CURRENT_TIMESTAMP),"
        "username TEXT UNIQUE NOT NULL, "
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
        "created TEXT DEFAULT (CURRENT_TIMESTAMP),"
        "level TEXT NOT NULL, "
        "description TEXT NOT NULL)";
    retval = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (retval != SQLITE_OK) {
        log_error("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
    } else {
        log_trace("Table \""LOG_TABLE"\" created successfully.");
    }

    return retval;
}

int db_close() {
    return sqlite3_close(db);
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

char *db_get_tables() {
    char *query = "SELECT name FROM sqlite_master WHERE type='table' and name not like 'sqlite_%' and name not like 'cb_%'";
    char *json = db_query(query);
    return json;
}

char *db_get_table(char *table_name) {
    char *query = malloc(16 + sizeof(table_name));
    snprintf(query, strlen(query), "SELECT * FROM %s", table_name);
    char *json = db_query(query);
    free(query);
    return json;
}

char *db_get_logs() {
    char *query = "SELECT created, level, description FROM " LOG_TABLE " order by created desc";
    char *json = db_query(query);
    return json;
}

char *db_get_admins() {
    char *query = "SELECT id, username, email, avatar, created FROM " ADMIN_TABLE ;
    char *json = db_query(query);
    return json;
}

char *db_add_user(char *username, char *password) {
    
    log_info("Adding new user: %s", username);
    char* query = "INSERT INTO " USER_TABLE " (username, salt, password) VALUES (?,?,?)";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to prepare statement\"}";
    }

    if (sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind username parameter: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to bind username parameter\"}";
    }

    char salt[SALT_LENGTH+1];
    char *hash_pass = hash_password(password, salt);

    if (sqlite3_bind_text(stmt, 2, salt, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind username parameter: %s", sqlite3_errmsg(db));
        free(hash_pass); 
        return "{\"error\": \"Failed to bind username parameter\"}";
    }

    if (sqlite3_bind_text(stmt, 3, hash_pass, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind password parameter: %s", sqlite3_errmsg(db));
        free(hash_pass); 
        return "{\"error\": \"Failed to bind password parameter\"}";
    }

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        log_error("Failed to insert user: %s", sqlite3_errmsg(db));
        free(hash_pass); 
        return "{\"error\": \"Failed to insert user\"}";
	}
    
	if(sqlite3_finalize(stmt) != SQLITE_OK){
        log_error("Failed finalize statement: %s", sqlite3_errmsg(db));
        free(hash_pass); 
        return "{\"error\": \"Failed finalize statement:\"}";
    }
    
    free(hash_pass); 

    return "{\"error\": \"\"}";
}

char *db_login(char *username, char *password) {
    
    log_trace("User %s logging in", username);
    char *query = "select password, salt from " USER_TABLE " where username = ?";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to prepare statement\"}";
    }
    
    if (sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind username parameter: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to bind username parameter\"}";
    }

    if(sqlite3_step(stmt) != SQLITE_ROW) {
        log_error("Failed to get user row: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to get user row\"}";
	}

    char* hashed_password = sqlite3_column_text(stmt, 0);
    char* salt = sqlite3_column_text(stmt, 1);

    if(check_password(hashed_password, password, salt) != 0) {
        log_error("The passwords doesn't match");
        return "{\"error\": \"wrong password\"}";
    }

    unsigned long current_time = (unsigned long)time(NULL);
    JSON_Value *payload_value = json_value_init_object();
    JSON_Object *payload_object = json_value_get_object(payload_value);
    char *serialized_string = NULL;    
    json_object_set_string(payload_object, "sub", username);
    json_object_set_string(payload_object, "username", username);
    json_object_set_string(payload_object, "role", "USER");
    json_object_set_number(payload_object, "iat", current_time);
    json_object_set_number(payload_object, "exp", (current_time+JWT_EXPIRATION_TIME));
    const char *payload = json_serialize_to_string_pretty(payload_value);

    char *jwt = jwt_sign(JWT_DEFAULT_HEADER, payload, JWT_SECRET_KEY);
    
    json_free_serialized_string(serialized_string);
    json_value_free(payload_value);
    
    char *json = malloc(strlen(jwt) + 20);
    sprintf(json, "{\"token\": \"%s\"}", jwt);
    return json;
}

char *db_admin_login(char *username, char *password) {
    
    log_trace("Admin %s logging in", username);
    char *query = "select password, salt, email from " ADMIN_TABLE " where username = ?";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to prepare statement\"}";
    }
    
    if (sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind username parameter: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to bind username parameter\"}";
    }

    if(sqlite3_step(stmt) != SQLITE_ROW) {
        log_error("Failed to get user row: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Unknown admin\"}";
	}

    char* hashed_password = sqlite3_column_text(stmt, 0);
    char* salt = sqlite3_column_text(stmt, 1);

    char* email = sqlite3_column_text(stmt, 2);

    if(check_password(hashed_password, password, salt) != 0) {
        log_error("The passwords doesn't match");
        return "{\"error\": \"wrong password for this admin\"}";
    }

    unsigned long current_time = (unsigned long)time(NULL);
    JSON_Value *payload_value = json_value_init_object();
    JSON_Object *payload_object = json_value_get_object(payload_value);
    json_object_set_string(payload_object, "sub", username);
    json_object_set_string(payload_object, "role", "ADMIN");
    json_object_set_string(payload_object, "username", username);
    json_object_set_number(payload_object, "iat", current_time);
    json_object_set_number(payload_object, "exp", current_time+JWT_EXPIRATION_TIME);

    const char *payload = json_serialize_to_string_pretty(payload_value);

    char *jwt = jwt_sign(JWT_DEFAULT_HEADER, payload, JWT_SECRET_KEY);
    
    json_free_serialized_string(payload);
    json_value_free(payload_value);

    JSON_Value *json_value = json_value_init_object();
    JSON_Object *json_object = json_value_get_object(json_value);
    json_object_set_string(json_object, "token", jwt);
    json_object_set_string(json_object, "username", username);
    json_object_set_string(json_object, "email", email);
    const char *json = json_serialize_to_string_pretty(json_value);
    json_value_free(json_value);
    return json;
}

char *db_add_admin(char *username, char *password) {
    
    log_info("Adding new admin: %s", username);
    char* query = "INSERT INTO " ADMIN_TABLE " (username, salt, password) VALUES (?,?,?)";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to prepare statement\"}";
    }

    if (sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind username parameter: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to bind username parameter\"}";
    }

    char salt[SALT_LENGTH+1];
    char *hash_pass = hash_password(password, salt);

    if (sqlite3_bind_text(stmt, 2, salt, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind username parameter: %s", sqlite3_errmsg(db));
        free(hash_pass); 
        return "{\"error\": \"Failed to bind username parameter\"}";
    }

    if (sqlite3_bind_text(stmt, 3, hash_pass, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind password parameter: %s", sqlite3_errmsg(db));
        free(hash_pass); 
        return "{\"error\": \"Failed to bind password parameter\"}";
    }

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        log_error("Failed to insert user: %s", sqlite3_errmsg(db));
        free(hash_pass); 
        return "{\"error\": \"Failed to insert user\"}";
	}
    
	if(sqlite3_finalize(stmt) != SQLITE_OK){
        log_error("Failed finalize statement: %s", sqlite3_errmsg(db));
        free(hash_pass); 
        return "{\"error\": \"Failed finalize statement:\"}";
    }
    
    free(hash_pass); 

    return "{\"error\": \"\"}";
}

char *db_delete_admin(char *username) {
    log_info("Deleting admin: %s", username);
    char* query = "DELETE FROM " ADMIN_TABLE " WHERE condition id = ?";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        log_error("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to prepare statement\"}";
    }
    if (sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC) != SQLITE_OK) {
        log_error("Failed to bind username parameter: %s", sqlite3_errmsg(db));
        return "{\"error\": \"Failed to bind username parameter\"}";
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

void db_sqlite_log_callback(log_Event *ev) {
    const char *sql = "INSERT INTO " LOG_TABLE " (level, description) VALUES (?, ?);";
    sqlite3_stmt *stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
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
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    free(final_message);
}