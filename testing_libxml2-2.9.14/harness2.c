#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// AFL++ persistent mode requires this macro
#ifdef __AFL_COMPILER
#include <afl-fuzz.h>
#endif
// Function to process XML data
void process_xml(const char* data, size_t size) {
    xmlDocPtr doc = xmlReadMemory(data, size, "noname.xml", NULL, 0);
    if (doc != NULL) {
        // Process the document here if needed
        xmlFreeDoc(doc);
    }
}

int main(int argc, char **argv) {
#ifdef __AFL_COMPILER
    // AFL++ persistent mode
    __AFL_INIT();
    unsigned char *buf = __AFL_FUZZ_TESTCASE_BUF;  // must be after __AFL_INIT
    while (__AFL_LOOP(10000)) {
        int len = __AFL_FUZZ_TESTCASE_LEN;  // don't use the macro directly in the xmlReadMemory function
        process_xml((const char *)buf, len);
    }
#else
    // Standard mode for testing outside of AFL++
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <xml_file>\n", argv[0]);
        return 1;
    }
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(file_size);
    if (!buffer) {
        perror("Failed to allocate memory");
        fclose(file);
        return 1;
    }
    if (fread(buffer, 1, file_size, file) != file_size) {
        perror("Failed to read file");
        free(buffer);
        fclose(file);
        return 1;
    }
    fclose(file);
    process_xml(buffer, file_size);
    free(buffer);
#endif

    xmlCleanupParser();
    return 0;
}
