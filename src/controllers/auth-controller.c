#include "auth-controller.h"

#include "../libs/parson.h"
#include "../libs/sqlite3.h"
#include "../db/db.h"
#include "../utils/jwt.h"
#include "../utils/info.h"


char *auth_add_user(char *username, char *password) {
    
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

char *auth_login(char *username, char *password) {
    
    log_trace("User %s logging in", username);
    char *query = "select password, salt, email from " USER_TABLE " where username = ?";
    
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
    char* email = sqlite3_column_text(stmt, 2);

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

    JSON_Value *json_value = json_value_init_object();
    JSON_Object *json_object = json_value_get_object(json_value);
    json_object_set_string(json_object, "token", jwt);
    json_object_set_string(json_object, "username", username);
    json_object_set_string(json_object, "email", email);
    const char *json = json_serialize_to_string_pretty(json_value);
    json_value_free(json_value);
    return json;
}

char *auth_admin_login(char *username, char *password) {
    
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

char *auth_add_admin(char *username, char *password) {
    
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

char *auth_delete_admin(char *username) {
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