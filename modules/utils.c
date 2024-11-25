

#include "utils.h"

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
        fprintf(stderr, "Memory allocation failed\n");
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