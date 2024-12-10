#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>
#include <stdlib.h>

void fuzz_archive(const char *filename) {
    const char *output_filename = "test.tar";
    struct archive *a = archive_read_new();
    struct archive *aw = archive_write_new();
    struct archive_entry *entry;
    
    archive_read_support_filter_gzip(a);
    archive_read_support_format_all(a);
    archive_write_set_format_pax_restricted(aw);
   
    if (archive_read_open_filename(a, filename, 10240) != ARCHIVE_OK) {
        fprintf(stderr, "Error opening archive: %s\n", archive_error_string(a));
        archive_read_free(a);
        exit(1);
    }

    if (archive_write_open_filename(aw, output_filename) != ARCHIVE_OK) {
        fprintf(stderr, "Error opening output archive: %s\n", archive_error_string(aw));
        archive_read_free(a);
        archive_write_free(aw);
        exit(1);
    }

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
      
	printf("Copying: %s\n", archive_entry_pathname(entry));

        if (archive_write_header(aw, entry) != ARCHIVE_OK) {
            fprintf(stderr, "Error writing header for %s: %s\n",
            archive_entry_pathname(entry), archive_error_string(aw));
            continue; 
        }

        const size_t buffer_size = 8192;
        char *temp = malloc(buffer_size);
        ssize_t size;
        while ((size = archive_read_data(a, temp, buffer_size)) > 0) {
            if (archive_write_data(aw, temp, size) < 0) {
                fprintf(stderr, "Error writing data for %s: %s\n",
                        archive_entry_pathname(entry), archive_error_string(aw));
                break;
            }
        }
        free(temp);
    }
    archive_read_free(a);
    archive_write_free(aw);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <archive-file>\n", argv[0]);
        return 1;
    }
    fuzz_archive(argv[1]);
    return 0;
}
