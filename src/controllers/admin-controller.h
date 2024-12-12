#ifndef ADMIN_CONTROLLER_H
#define ADMIN_CONTROLLER_H


// TABLES
char *ctr_create_table(char* table_name, char* json_columns);
char *ctr_delete_table(char* table_name);
char *ctr_get_tables();
char *ctr_get_table(char *table_name, char* where_clause);

// LOGS
char *ctr_get_logs();

// CONFIGS
char *ctr_get_configs();
char *ctr_get_config_value(char *config_name);

#endif