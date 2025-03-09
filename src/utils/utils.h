#ifndef UTILS_H
#define UTILS_H

#include "../libs/sqlite3.h"


char *db_sqlite_to_json(sqlite3_stmt *stmt);

char *hash_password(char* password, char* salt);
int check_password(char *hashed_password, char* password, char* salt);

char *base64url_encode(const unsigned char *input, size_t length);
char *base64url_decode(const char *input);

char *error_json_char(const char *error);

char *get_config_str(const char *key);
int get_config_int(const char *key);
char* add_config(const char *key, const char *value, char *type);
char* update_config(const char *key, const char *value);
char* delete_config(const char *key);

#endif