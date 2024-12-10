#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>
#include <stdlib.h>

//the goal with this test is to check for errors with reading and writing between archives in libarchive
void read_write_fuzz_harness(const char *input_archive) {
	const char *output_archive = "harness_test.tar";
	struct archive *in = archive_read_new();
	struct archive *out = archive_write_new();
	struct archive_entry *entry;

	// Enable specific formats and filters
	archive_read_support_filter_all(in);
	archive_read_support_format_all(in);
	archive_read_support_format_empty(in);
	//basic checks because we don't want the harness to crash because of user error - just if there is a bug in the api
	if (archive_read_open_filename(in, input_archive, 10240) != ARCHIVE_OK) {
    	fprintf(stderr, "Error opening archive: %s\n", archive_error_string(in));
    	archive_read_free(in);
    	exit(1);
	}

	// Set output format
	archive_write_set_format_pax_restricted(out);

	if (archive_write_open_filename(out, output_archive) != ARCHIVE_OK) {
    	fprintf(stderr, "Error opening output archive: %s\n", archive_error_string(out));
    	archive_read_free(in);
    	archive_write_free(out);
    	exit(1);
	}

	// Process each entry in the input archive
	while (archive_read_next_header(in, &entry) == ARCHIVE_OK) {
    	printf("Copying: %s\n", archive_entry_pathname(entry));

    	if (archive_write_header(out, entry) != ARCHIVE_OK) {
        	fprintf(stderr, "Error writing header for %s: %s\n",
        	archive_entry_pathname(entry), archive_error_string(out));
        	continue;
    	}

    	// Allocate buffer for data transfer
    	const size_t buffer_size = 8192;
    	char *buff = malloc(buffer_size);
    	if (!buff) {
        	fprintf(stderr, "Memory allocation error\n");
        	break;
    	}

    	// Copy data over
    	ssize_t size;
    	while ((size = archive_read_data(in, buff, buffer_size)) > 0) {
        	if (archive_write_data(out, buff, size) < 0) {
            	fprintf(stderr, "Error writing data for %s: %s\n",
                    	archive_entry_pathname(entry), archive_error_string(out));
            	break;
        	}
    	}
    	if (size < 0) {
        	fprintf(stderr, "Error reading data for %s: %s\n",
                	archive_entry_pathname(entry), archive_error_string(in));
    	}


    	free(buff);
    	if (archive_write_finish_entry(out) != ARCHIVE_OK) {
        	fprintf(stderr, "Error finishing entry for %s: %s\n",
                	archive_entry_pathname(entry), archive_error_string(out));
    	}
	}
	//clean up clean up everybody clap their hands
	archive_read_free(in);
	archive_write_free(out);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
    	fprintf("Please give the path and name of a archive file\n", argv[0]);
    	return 1;
	}

	read_write_fuzz_harness(argv[1]);
	printf("Archive test complete.\n");
	return 0;
}
