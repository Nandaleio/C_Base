

#include <stddef.h>
#include <stdio.h>

#include "utils.h"
#include "info.h"
#include "../db/db.h"
#include "../libs/log.h"
#include "../libs/mongoose.h"
#include "../libs/parson.h"

/**
 * Converts an SQLite result set to a JSON string.
 *
 * @param stmt An SQLite prepared statement with the result set.
 * @return A dynamically allocated JSON string, or NULL on failure.
 */
char *db_sqlite_to_json(sqlite3_stmt *stmt) {
    if (!stmt) {
        log_error( "Invalid SQLite statement or database");
        return "{\"columns\":[],\"data\":[],\"error\":\"Invalid input\"}";
    }

    // Start building the JSON
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    
    // Add columns array
    JSON_Value *columns_value = json_value_init_array();
    JSON_Array *columns_array = json_value_get_array(columns_value);
    json_object_set_value(root_object, "columns", columns_value);

    int col_count = sqlite3_column_count(stmt);
    for (int i = 0; i < col_count; i++) {
        JSON_Value *row_value = json_value_init_object();
        JSON_Object *row_object = json_value_get_object(row_value);
        const char *col_name = sqlite3_column_name(stmt, i);
        const char *col_type = sqlite3_column_decltype(stmt, i);
        json_object_set_string(row_object, "name", col_name);
        json_object_set_string(row_object, "type", col_type);
        json_array_append_value(columns_array, row_value);
    }

    // Add data array
    JSON_Value *data_value = json_value_init_array();
    JSON_Array *data_array = json_value_get_array(data_value);
    json_object_set_value(root_object, "data", data_value);

    int row_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        JSON_Value *row_value = json_value_init_object();
        JSON_Object *row_object = json_value_get_object(row_value);

        for (int i = 0; i < col_count; i++) {
            const char *col_name = sqlite3_column_name(stmt, i);
            int col_type = sqlite3_column_type(stmt, i);

            switch (col_type) {
                case SQLITE_FLOAT:
                case SQLITE_INTEGER:
                    json_object_set_number(row_object, col_name, sqlite3_column_int64(stmt, i));
                    break;
                case SQLITE_TEXT:
                    json_object_set_string(row_object, col_name, (const char*)sqlite3_column_text(stmt, i));
                    break;
                case SQLITE_NULL:
                    json_object_set_null(row_object, col_name);
                    break;
                default:
                    json_object_set_string(row_object, col_name, "Unsupported type");
            }
        }

        json_array_append_value(data_array, row_value);
        row_count++;

        // if (sqlite3_step(stmt) == SQLITE_ROW) {
        //     sqlite3_reset(stmt); // Reset to re-fetch this row
        // } else {
        //     break;
        // }
    }

    // Add error field
    const char *error_msg = sqlite3_errmsg(db);
    if (row_count == 0 && strlen(error_msg) > 0) {
        json_object_set_string(root_object, "error", error_msg);
    } else {
        json_object_set_null(root_object, "error");
    }

    // Serialize to string
    char *json_str = json_serialize_to_string(root_value);
    json_value_free(root_value);

    return json_str;
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

static const char base64url_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
char *base64url_encode(const unsigned char *input, size_t length) {
    size_t output_length = 4 * ((length + 2) / 3); // Calculate output length
    char *output = malloc(output_length + 1);     // Allocate memory for encoded string
    if (!output) return NULL;                     // Memory allocation failed

    size_t i, j;
    for (i = 0, j = 0; i < length;) {
        uint32_t octet_a = i < length ? input[i++] : 0;
        uint32_t octet_b = i < length ? input[i++] : 0;
        uint32_t octet_c = i < length ? input[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        output[j++] = base64url_chars[(triple >> 18) & 0x3F];
        output[j++] = base64url_chars[(triple >> 12) & 0x3F];
        output[j++] = base64url_chars[(triple >> 6) & 0x3F];
        output[j++] = base64url_chars[triple & 0x3F];
    }

    // Apply padding rules for Base64URL
    size_t mod = length % 3;
    if (mod > 0) {
        output[output_length - 1] = '\0';
        if (mod == 1) {
            output[--j] = '\0';
            output[--j] = '\0';
        } else if (mod == 2) {
            output[--j] = '\0';
        }
    }
    output[j] = '\0'; // Null-terminate the string
    return output;
}

char *base64url_decode(const char *input) {
    size_t input_length = strlen(input);
    if (input_length % 4 != 0 && input_length % 4 != 2 && input_length % 4 != 3) {
        return NULL; // Invalid base64url string
    }

    size_t len = input_length;
    size_t padding = 0;
    if (len > 0) {
        if (input[len-1] == '\0') padding++;
        if (len > 1 && input[len-2] == '\0') padding++;
    }

    size_t output_length = (len * 3) / 4 - padding;
    unsigned char *output = malloc(output_length);
    if (!output) return NULL;

    size_t i, j;
    uint32_t sextet_a, sextet_b, sextet_c, sextet_d;
    uint32_t triple;

    for (i = 0, j = 0; i < len;) {
        sextet_a = input[i] == '\0' ? 0 & i++ : strchr(base64url_chars, input[i++]) - base64url_chars;
        sextet_b = input[i] == '\0' ? 0 & i++ : strchr(base64url_chars, input[i++]) - base64url_chars;
        sextet_c = input[i] == '\0' ? 0 & i++ : strchr(base64url_chars, input[i++]) - base64url_chars;
        sextet_d = input[i] == '\0' ? 0 & i++ : strchr(base64url_chars, input[i++]) - base64url_chars;

        triple = (sextet_a << 18) + (sextet_b << 12) + (sextet_c << 6) + sextet_d;

        if (j < output_length) output[j++] = (triple >> 16) & 0xFF;
        if (j < output_length) output[j++] = (triple >> 8) & 0xFF;
        if (j < output_length) output[j++] = triple & 0xFF;
    }

    return output;
}
