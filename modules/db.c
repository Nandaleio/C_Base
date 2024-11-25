

#include "db.h"

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

int db_init() {
    
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    sqlite3_stmt* stmt = NULL;

    int retval;
    char *sql = "CREATE TABLE IF NOT EXISTS admin ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
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

    sql = "CREATE TABLE IF NOT EXISTS user ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created TEXT DEFAULT (CURRENT_TIMESTAMP),"
        "username TEXT NOT NULL, "
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

    return retval;
}

int db_close() {
    return sqlite3_close(db);
}

/**
 * Converts an SQLite result set to a JSON string.
 *
 * @param stmt An SQLite prepared statement with the result set.
 * @return A dynamically allocated JSON string, or NULL on failure.
 */
char *db_sqlite_to_json(sqlite3_stmt *stmt) {
    if (!stmt) {
        fprintf(stderr, "Invalid SQLite statement\n");
        return NULL;
    }

    // Start building the JSON array
    size_t buffer_size = 256; // Initial size
    char *json = malloc(buffer_size);
    if (!json) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    strcpy(json, "["); // Start of the JSON array
    size_t json_len = 1; // Track the length of the JSON string

    int row_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int col_count = sqlite3_column_count(stmt);
        size_t row_buffer_size = 256; // Initial size for a single row
        char *row_json = malloc(row_buffer_size);
        if (!row_json) {
            fprintf(stderr, "Memory allocation failed for row\n");
            free(json);
            return NULL;
        }
        strcpy(row_json, "{");
        size_t row_len = 1;

        // Iterate over columns in the row
        for (int i = 0; i < col_count; i++) {
            const char *col_name = sqlite3_column_name(stmt, i);
            int col_type = sqlite3_column_type(stmt, i);
            char col_value[256] = {0};

            switch (col_type) {
                case SQLITE_INTEGER:
                    snprintf(col_value, sizeof(col_value), "%lld", sqlite3_column_int64(stmt, i));
                    break;
                case SQLITE_FLOAT:
                    snprintf(col_value, sizeof(col_value), "%g", sqlite3_column_double(stmt, i));
                    break;
                case SQLITE_TEXT:
                    snprintf(col_value, sizeof(col_value), "\"%s\"", sqlite3_column_text(stmt, i));
                    break;
                case SQLITE_NULL:
                    strcpy(col_value, "null");
                    break;
                default:
                    fprintf(stderr, "Unsupported column type\n");
                    free(row_json);
                    free(json);
                    return NULL;
            }

            // Resize the row buffer if needed
            size_t additional_len = strlen(col_name) + strlen(col_value) + 4; // Quotes, colon, comma
            if (row_len + additional_len + 1 > row_buffer_size) {
                row_buffer_size *= 2;
                char *temp = realloc(row_json, row_buffer_size);
                if (!temp) {
                    fprintf(stderr, "Memory reallocation failed for row\n");
                    free(row_json);
                    free(json);
                    return NULL;
                }
                row_json = temp;
            }

            // Append the column name and value
            strcat(row_json, "\"");
            strcat(row_json, col_name);
            strcat(row_json, "\":");
            strcat(row_json, col_value);
            row_len += additional_len;

            // Add a comma if not the last column
            if (i < col_count - 1) {
                strcat(row_json, ",");
                row_len++;
            }
        }

        strcat(row_json, "}"); // End of the row JSON object
        row_len++;

        // Resize the main JSON buffer if needed
        if (json_len + row_len + 2 > buffer_size) { // +2 for comma and null terminator
            buffer_size *= 2;
            char *temp = realloc(json, buffer_size);
            if (!temp) {
                fprintf(stderr, "Memory reallocation failed for JSON\n");
                free(row_json);
                free(json);
                return NULL;
            }
            json = temp;
        }

        // Append the row JSON to the main JSON
        strcat(json, row_json);
        json_len += row_len;

        free(row_json);

        // Add a comma if not the last row
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            strcat(json, ",");
            json_len++;
            sqlite3_reset(stmt); // Reset to re-fetch this row
        } else {
            break; // No more rows
        }

        row_count++;
    }

    strcat(json, "]"); // End of the JSON array

    if (row_count == 0) { // No rows found
        strcpy(json, "[]");
    }

    return json;
}

char *db_query(char* query) {
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return "{}";
    }

    char *json = db_sqlite_to_json(stmt);
    if (json) {
        printf("JSON Result:\n%s\n", json);
    } else {
        fprintf(stderr, "Failed to convert result set to JSON\n");
    }

    sqlite3_finalize(stmt);

    return json;
}

char *login(char* username, char* password) {
    return "todo";
}
