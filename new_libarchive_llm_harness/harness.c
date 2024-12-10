
#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>
#include <stdlib.h>

void list_archive(const char *filename) {
    struct archive *a;
    struct archive_entry *entry;
    int r;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    if ((r = archive_read_open_filename(a, filename, 10240))) {
        fprintf(stderr, "Error opening archive: %s\n", archive_error_string(a));
        exit(1);
    }

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        printf("Entry: %s\n", archive_entry_pathname(entry));
        archive_read_data_skip(a);
    }

    r = archive_read_free(a);
    if (r != ARCHIVE_OK) {
        fprintf(stderr, "Error closing archive: %s\n", archive_error_string(a));
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <archive-file>\n", argv[0]);
        return 1;
    }

    list_archive(argv[1]);
    return 0;
}