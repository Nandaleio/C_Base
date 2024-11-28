

#include "db.h"
#include <stddef.h>
#include <stdio.h>
#include "utils.h"
#include "../libs/log.h"
#include "../libs/mongoose.h"

char *convert_array_to_json(char **array, int array_size) {
    if (array == NULL || array_size <= 0) {
        char *empty_json = strdup("[]");
        return empty_json; // Return an empty JSON array if no tables
    }

    // Estimate initial buffer size (each name + quotes + comma)
    size_t buffer_size = 2; // Start with 2 for '[' and ']'
    for (int i = 0; i < array_size; i++) {
        buffer_size += strlen(array[i]) + 3; // Quotes + comma
    }

    // Allocate the buffer
    char *json = malloc(buffer_size);
    if (json == NULL) {
        log_error("Memory allocation failed");
        return NULL;
    }

    // Start building the JSON string
    strcpy(json, "[");
    for (int i = 0; i < array_size; i++) {
        strcat(json, "\"");
        strcat(json, array[i]);
        strcat(json, "\"");
        if (i < array_size - 1) {
            strcat(json, ",");
        }
    }
    strcat(json, "]");

    return json;
}

/**
 * Converts an SQLite result set to a JSON string.
 *
 * @param stmt An SQLite prepared statement with the result set.
 * @return A dynamically allocated JSON string, or NULL on failure.
 */
char *db_sqlite_to_json(sqlite3_stmt *stmt) {
    if (!stmt) {
        log_error( "Invalid SQLite statement or database");
        return strdup("{\"columns\":[],\"data\":[],\"error\":\"Invalid input\"}");
    }

    // Start building the JSON
    size_t buffer_size = 512;
    char *json = malloc(buffer_size);
    if (!json) {
        log_error("Memory allocation failed");
        return strdup("{\"columns\":[],\"data\":[],\"error\":\"Memory allocation failed\"}");
    }
    strcpy(json, "{");
    size_t json_len = strlen(json);

    // Columns section
    strcat(json, "\"columns\":[");
    json_len += 11;

    int col_count = sqlite3_column_count(stmt);
    for (int i = 0; i < col_count; i++) {
        const char *col_name = sqlite3_column_name(stmt, i);
        size_t col_name_len = strlen(col_name) + 3; // Quotes and comma
        if (json_len + col_name_len + 1 > buffer_size) {
            buffer_size *= 2;
            char *temp = realloc(json, buffer_size);
            if (!temp) {
                free(json);
                return strdup("{\"columns\":[],\"data\":[],\"error\":\"Memory allocation failed\"}");
            }
            json = temp;
        }
        strcat(json, "\"");
        strcat(json, col_name);
        strcat(json, "\"");
        json_len += col_name_len;
        if (i < col_count - 1) {
            strcat(json, ",");
            json_len++;
        }
    }
    strcat(json, "],\"data\":[");
    json_len += 10;

    // Data section
    int row_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        size_t row_buffer_size = 256; // Initial size for a single row
        char *row_json = malloc(row_buffer_size);
        if (!row_json) {
            free(json);
            return strdup("{\"columns\":[],\"data\":[],\"error\":\"Memory allocation failed\"}");
        }
        strcpy(row_json, "{");
        size_t row_len = 1;

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
                    strcpy(col_value, "\"Unsupported type\"");
            }

            size_t additional_len = strlen(col_name) + strlen(col_value) + 4; // Quotes, colon, comma
            if (row_len + additional_len + 1 > row_buffer_size) {
                row_buffer_size *= 2;
                char *temp = realloc(row_json, row_buffer_size);
                if (!temp) {
                    free(row_json);
                    free(json);
                    return strdup("{\"columns\":[],\"data\":[],\"error\":\"Memory allocation failed\"}");
                }
                row_json = temp;
            }

            strcat(row_json, "\"");
            strcat(row_json, col_name);
            strcat(row_json, "\":");
            strcat(row_json, col_value);
            row_len += additional_len;

            if (i < col_count - 1) {
                strcat(row_json, ",");
                row_len++;
            }
        }

        strcat(row_json, "}");
        row_len++;

        if (json_len + row_len + 2 > buffer_size) { // Resize JSON buffer if needed
            buffer_size *= 2;
            char *temp = realloc(json, buffer_size);
            if (!temp) {
                free(row_json);
                free(json);
                return strdup("{\"columns\":[],\"data\":[],\"error\":\"Memory allocation failed\"}");
            }
            json = temp;
        }

        strcat(json, row_json);
        json_len += row_len;
        free(row_json);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            strcat(json, ",");
            json_len++;
            sqlite3_reset(stmt); // Reset to re-fetch this row
        } else {
            break; // No more rows
        }
        row_count++;
    }

    strcat(json, "],\"error\":");
    json_len += 9;

    // Error section
    const char *error_msg = sqlite3_errmsg(db);
    if (row_count == 0 && strlen(error_msg) > 0) {
        size_t error_len = strlen(error_msg) + 3; // Quotes
        if (json_len + error_len + 1 > buffer_size) {
            buffer_size *= 2;
            char *temp = realloc(json, buffer_size);
            if (!temp) {
                free(json);
                return strdup("{\"columns\":[],\"data\":[],\"error\":\"Memory allocation failed\"}");
            }
            json = temp;
        }
        strcat(json, "\"");
        strcat(json, error_msg);
        strcat(json, "\"");
    } else {
        strcat(json, "null");
    }

    strcat(json, "}"); // Close JSON object

    return json;
}

char *hash_password(char* password, char* salt) {
    mg_random_str(salt, SALT_LENGTH+1);
    mg_sha256_ctx ctx;
    mg_sha256_init(&ctx);
    mg_sha256_update(&ctx, password, strlen(password));
    mg_sha256_update(&ctx, salt, strlen(salt));
    unsigned char *hash_pass = malloc(32);
    mg_sha256_final(hash_pass, &ctx);
    return hash_pass;
}

int check_password(char *hashed_password, char* password, char* salt) {
    mg_sha256_ctx ctx;
    mg_sha256_init(&ctx);
    mg_sha256_update(&ctx, password, strlen(password));
    mg_sha256_update(&ctx, salt, strlen(salt));
    unsigned char *hash_pass = malloc(32);
    mg_sha256_final(hash_pass, &ctx);
    int ret = memcmp(hashed_password, hash_pass, 32);
    free(hash_pass);
    return ret;
}

char *base64_encode(const unsigned char *input, int length) {
    static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char *encoded = malloc((length + 2) / 3 * 4 + 1);
    if (!encoded) return NULL;

    char *p = encoded;
    for (int i = 0; i < length; i += 3) {
        int a = input[i];
        int b = i + 1 < length ? input[i + 1] : 0;
        int c = i + 2 < length ? input[i + 2] : 0;

        *p++ = b64_table[a >> 2];
        *p++ = b64_table[((a & 3) << 4) | (b >> 4)];
        *p++ = b64_table[((b & 15) << 2) | (c >> 6)];
        *p++ = b64_table[c & 63];
    }

    while ((p - encoded) % 4) *p++ = '=';
    *p = '\0';
    return encoded;
}