
#include "admin-controller.h"
#include "../db/db.h"
#include "../libs/parson.h"


// TABLES :

char *ctr_create_table(char* table_name, char* json_columns) {

    JSON_Value *root_value = json_parse_string(json_columns);
    if (root_value == NULL) {
        return "{\"error\": \"Invalid JSON format\"}";
    }

    JSON_Array *columns = json_value_get_array(root_value);
    if (columns == NULL) {
        json_value_free(root_value);
        return "{\"error\": \"JSON must be an array\"}";
    }

    size_t column_count = json_array_get_count(columns);
    if (column_count == 0) {
        json_value_free(root_value);
        return "{\"error\": \"No columns specified\"}";
    }

    // Calculate total length needed for table_columns
    size_t total_length = 0;
    for (size_t i = 0; i < column_count; i++) {
        JSON_Object *column = json_array_get_object(columns, i);
        const char *name = json_object_get_string(column, "name");
        const char *type = json_object_get_string(column, "type");
        if (!name || !type) {
            json_value_free(root_value);
            return "{\"error\": \"Column must have name and type\"}";
        }
        total_length += strlen(name) + strlen(type) + 2; // +2 for space and comma
    }

    char table_columns[total_length + 1];
    table_columns[0] = '\0';

    // Build the columns string
    for (size_t i = 0; i < column_count; i++) {
        JSON_Object *column = json_array_get_object(columns, i);
        const char *name = json_object_get_string(column, "name");
        const char *type = json_object_get_string(column, "type");
        
        strcat(table_columns, name);
        strcat(table_columns, " ");
        strcat(table_columns, type);
        if (i < column_count - 1) {
            strcat(table_columns, ",");
        }
    }

    json_value_free(root_value);

    char query[strlen(table_name) + strlen(table_columns) + 16];
    strcpy(query, "CREATE TABLE ");
    strcat(query, table_name);
    strcat(query, "(");
    strcat(query, table_columns);
    strcat(query, ")");
    char *json = db_query(query);
    free(query);
    return json;
}

char *ctr_delete_table(char* table_name) {
    char query[12 + strlen(table_name)];
    strcpy(query, "DROP TABLE ");
    strcat(query, table_name);
    char *json = db_query(query);
    return json;
}

char *ctr_get_tables() {
    char *query = "SELECT name FROM sqlite_master WHERE type='table' and name not like 'sqlite_%' and name not like 'cb_%'";
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

char *ctr_get_config_value(char *config_name){
    char *query = "SELECT * FROM " CONFIG_TABLE " WHERE name = ?";
    char *json = db_query_param(query, config_name);
    return json;
}