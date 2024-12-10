
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

void fuzz_cJSON_Parse(const char *json_data) {
    cJSON *json = cJSON_Parse(json_data);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return;
    }
    cJSON_Delete(json);
}

void fuzz_cJSON_Print(const char *json_data) {
    cJSON *json = cJSON_Parse(json_data);
    if (json == NULL) {
        return;
    }
    char *printed_json = cJSON_Print(json);
    if (printed_json) {
        printf("%s\n", printed_json);
        free(printed_json);
    }
    cJSON_Delete(json);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <json_string>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *json_data = argv[1];
    
    fuzz_cJSON_Parse(json_data);
    fuzz_cJSON_Print(json_data);
    
    return EXIT_SUCCESS;
}