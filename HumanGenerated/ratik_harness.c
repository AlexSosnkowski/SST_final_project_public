#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For mkstemp

#define ARCHIVE_OPT 10240
#define MAX_ENTRIES 5

typedef struct archive archive_t;
typedef struct archive_entry entry_t;

void read_archive(const char *filename) {
    archive_t *a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    if (archive_read_open_filename(a, filename, ARCHIVE_OPT) != ARCHIVE_OK) {
        fprintf(stderr, "Failed to open archive: %s\n", archive_error_string(a));
        archive_read_free(a);
        return;
    }

    entry_t *entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        printf("Read entry: %s\n", archive_entry_pathname(entry));
    }

    archive_read_free(a);
}

void write_archive_from_input(const char *filename, const char *input_data, size_t input_size) {
    archive_t *a = archive_write_new();
    archive_write_set_format_zip(a);
    archive_write_open_filename(a, filename);

    size_t chunk_size = input_size / MAX_ENTRIES;
    for (int i = 0; i < MAX_ENTRIES && chunk_size > 0; i++) {
        size_t offset = i * chunk_size;
        size_t data_size = (offset + chunk_size <= input_size) ? chunk_size : input_size - offset;

        entry_t *entry = archive_entry_new();
        char entry_name[64];
        snprintf(entry_name, sizeof(entry_name), "file_%d.txt", i);
        archive_entry_set_pathname(entry, entry_name);
        archive_entry_set_size(entry, data_size);
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);

        archive_write_header(a, entry);
        archive_write_data(a, input_data + offset, data_size);
        archive_entry_free(entry);
    }

    archive_write_close(a);
    archive_write_free(a);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_archive>\n", argv[0]);
        return 1;
    }

    // Step 1: Read input archive using AFL input seed
    printf("Step 1: Reading input archive\n");
    read_archive(argv[1]);

    // Use a temporary file for the generated archive
    char tmp_filename[] = "/tmp/fuzz_archive_XXXXXX";
    int fd = mkstemp(tmp_filename);
    if (fd == -1) {
        perror("Failed to create temporary file");
        return 1;
    }
    close(fd);

    // Read the input file to use its contents for writing new entries
    FILE *input_file = fopen(argv[1], "rb");
    if (!input_file) {
        perror("Failed to open input file");
        return 1;
    }

    fseek(input_file, 0, SEEK_END);
    long input_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    char *input_data = malloc(input_size);
    if (!input_data) {
        perror("Failed to allocate buffer for input data");
        fclose(input_file);
        return 1;
    }

    fread(input_data, 1, input_size, input_file);
    fclose(input_file);

    printf("\nStep 2: Writing generated archive based on input data\n");
    write_archive_from_input(tmp_filename, input_data, input_size);

    printf("\nStep 3: Reading back generated archive\n");
    read_archive(tmp_filename);

    free(input_data);
    unlink(tmp_filename);  // Clean up temporary file
    return 0;
}

