#ifndef UTILS_H
#define UTILS_H

#include "../libs/sqlite3.h"


char *db_sqlite_to_json(sqlite3_stmt *stmt);

char *hash_password(char* password, char* salt);
int check_password(char *hashed_password, char* password, char* salt);

char *base64url_encode(const unsigned char *input, size_t length);
char *base64url_decode(const char *input);

#endif