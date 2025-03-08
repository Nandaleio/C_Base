
#include "admin-controller.h"

#include <string.h>

#include "../db/db.h"
#include "../libs/parson.h"
#include "../utils/utils.h"

// TABLES :

char *ctr_create_table(const char *table_name, const char *json_schema) {
    JSON_Value *root_value = json_parse_string(json_schema);
    if (!root_value) {
        log_error("Failed to parse JSON schema.\n");
        return error_json_char("Failed to parse JSON schema.");
    }

    JSON_Array *columns = json_value_get_array(root_value);
    if (!columns) {
        log_error("Invalid JSON schema format.\n");
        json_value_free(root_value);
        return error_json_char("Invalid JSON schema format.");
    }

    size_t column_count = json_array_get_count(columns);
    if (column_count == 0) {
        log_error("No columns provided in schema.\n");
        json_value_free(root_value);
        return error_json_char("No columns provided in schema.");
    }

    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "CREATE TABLE %s (", table_name);

    for (size_t i = 0; i < column_count; i++) {
        JSON_Object *column = json_array_get_object(columns, i);
        if (!column) {
            log_error("Invalid column definition.\n");
            json_value_free(root_value);
            return error_json_char("Invalid column definition");
        }

        const char *name = json_object_get_string(column, "name");
        const char *type = json_object_get_string(column, "type");
        JSON_Object *options = json_object_get_object(column, "options");

        if (!name || !type) {
            log_error("Column name or type missing.\n");
            json_value_free(root_value);
            return error_json_char("Column name or type missing.");
        }

        strncat(sql, name, sizeof(sql) - strlen(sql) - 1);
        strncat(sql, " ", sizeof(sql) - strlen(sql) - 1);
        strncat(sql, type, sizeof(sql) - strlen(sql) - 1);

        if (options) {
            if (json_object_has_value(options, "primary") && json_object_get_boolean(options, "primary")) {
                strncat(sql, " PRIMARY KEY", sizeof(sql) - strlen(sql) - 1);
                if (strcmp(type, "INTEGER") == 0 && json_object_has_value(options, "autoincrement") && json_object_get_boolean(options, "autoincrement")) {
                    strncat(sql, " AUTOINCREMENT", sizeof(sql) - strlen(sql) - 1);
                }
            }
            if (json_object_has_value(options, "unique") && json_object_get_boolean(options, "unique")) {
                strncat(sql, " UNIQUE", sizeof(sql) - strlen(sql) - 1);
            }
            if (json_object_has_value(options, "notnull") && json_object_get_boolean(options, "notnull")) {
                strncat(sql, " NOT NULL", sizeof(sql) - strlen(sql) - 1);
            }
            
            const char *default_value = json_object_get_string(options, "default");
            if (default_value) {
                strncat(sql, " DEFAULT (", sizeof(sql) - strlen(sql) - 1);
                strncat(sql, default_value, sizeof(sql) - strlen(sql) - 1);
                strncat(sql, ")", sizeof(sql) - strlen(sql) - 1);
            }

            const char *foreign = json_object_get_string(options, "foreign");
            if(foreign) {
                strncat(sql, " REFERENCES ", sizeof(sql) - strlen(sql) - 1);
                strncat(sql, foreign, sizeof(sql) - strlen(sql) - 1);
                strncat(sql, " ON DELETE ", sizeof(sql) - strlen(sql) - 1);
                if(json_object_has_value(options, "unique") && json_object_get_boolean(options, "unique")) {
                    strncat(sql, "SET NULL", sizeof(sql) - strlen(sql) - 1);
                } else {
                    strncat(sql, "CASCADE", sizeof(sql) - strlen(sql) - 1);
                }
            }


            const char *check = json_object_get_string(options, "check");
            if (check) {
                strncat(sql, " CHECK (", sizeof(sql) - strlen(sql) - 1);
                strncat(sql, check, sizeof(sql) - strlen(sql) - 1);
                strncat(sql, ")", sizeof(sql) - strlen(sql) - 1);
            }
        }

        if (i < column_count - 1) {
            strncat(sql, ", ", sizeof(sql) - strlen(sql) - 1);
        }
    }


    json_value_free(root_value);
    strncat(sql, ") STRICT", sizeof(sql) - strlen(sql) - 1);
    char *json = db_query(sql);
    return json;
}

char *ctr_delete_table(char* table_name) {
    char query[12 + strlen(table_name)];
    strcpy(query, "DROP TABLE ");
    strcat(query, table_name);
    char *json = db_query(query);
    return json;
}

// LOGS

char *ctr_get_logs() {
    char *query = "SELECT created, level, description FROM " LOG_TABLE " order by created desc";
    char *json = db_query(query);
    return json;
}

// ADMINS :

char *ctr_get_admins() {
    char *query = "SELECT id, username, email, avatar, created FROM " ADMIN_TABLE ;
    char *json = db_query(query);
    return json;
}

// CONFIGS :

char *ctr_get_configs(){
    char *query = "SELECT * FROM " CONFIG_TABLE;
    char *json = db_query(query);
    return json;
}