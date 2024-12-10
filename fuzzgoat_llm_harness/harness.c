#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "fuzzgoat.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <json_file>\n", argv[0]);
        return 1;
    }

    // Open the JSON file
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("fopen");
        return 1;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the entire file into a buffer
    char *data = malloc(length + 1);
    if (!data) {
        perror("malloc");
        fclose(file);
        return 1;
    }
    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';  // Ensure null-termination

    // Parse the JSON data
    json_settings settings = { 0 };
    char error[json_error_max];
    json_value *value = json_parse_ex(&settings, data, length, error);

    if (!value) {
        fprintf(stderr, "Error parsing JSON: %s\n", error);
        free(data);
        return 1;
    }

    // Free the JSON value
    json_value_free(value);

    free(data);
    return 0;
}

