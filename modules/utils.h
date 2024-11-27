#ifndef UTILS_H
#define UTILS_H

#include "../libs/sqlite3.h"

#define SALT_LENGTH 20

char *convert_array_to_json(char **array, int array_size);
char *db_sqlite_to_json(sqlite3_stmt *stmt);

char *hash_password(char* password, char* salt);
int check_password(char *hashed_password, char* password, char* salt);

char *base64_encode(const unsigned char *input, int length);

#endif