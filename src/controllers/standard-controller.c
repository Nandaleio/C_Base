
#include "admin-controller.h"
#include <stddef.h>


char *ctr_get_table(char *table_name, char* where_clause) {
    size_t query_size = 15 + strlen(table_name);
    if(where_clause != NULL) query_size += strlen(where_clause);
    char query[query_size];
    strcpy(query, "SELECT * FROM ");
    strcat(query, table_name);
    if(where_clause != NULL) strcat(query, where_clause);
    char *json = db_query(query);
    return json;
}