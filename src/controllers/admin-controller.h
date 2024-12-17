#ifndef ADMIN_CONTROLLER_H
#define ADMIN_CONTROLLER_H


// TABLES
char *ctr_create_table(char* table_name, char* json_columns);
char *ctr_delete_table(char* table_name);
char *ctr_get_tables();

// LOGS
char *ctr_get_logs();

// CONFIGS
char *ctr_get_configs();
char *ctr_set_config_value(char *config_name, char* config_value);

char *ctr_get_config_value_str(char *config_name);
bool ctr_get_config_value_boolean(char *config_name);
int ctr_get_config_value_int(char *config_name);
double ctr_get_config_value_double(char *config_name);

#endif