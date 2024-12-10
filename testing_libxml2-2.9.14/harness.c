#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>

// Function to recursively print XML elements
void print_element_names(xmlNode *node) {
    for (xmlNode *cur = node; cur; cur = cur->next) {
        if (cur->type == XML_ELEMENT_NODE) {
            printf("Node name: %s\n", cur->name);
        }
        print_element_names(cur->children);
    }
}

// Function to process an XML file
void process_xml(const char *filename) {
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    // Parse the XML file
    doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse XML file: %s\n", filename);
        return;
    }

    // Get the root element of the XML document
    root_element = xmlDocGetRootElement(doc);
    if (root_element == NULL) {
        fprintf(stderr, "Empty XML document: %s\n", filename);
        xmlFreeDoc(doc);
        return;
    }

    // Print the name of the root element
    printf("Root element: %s\n", root_element->name);

    // Traverse and print the XML elements
    print_element_names(root_element);

    // Free the XML document
    xmlFreeDoc(doc);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <xml-file>\n", argv[0]);
        return 1;
    }

    // Initialize the libxml2 library
    LIBXML_TEST_VERSION;

    // Process the specified XML file
    process_xml(argv[1]);

    // Clean up memory allocated by the library
    xmlCleanupParser();

    return 0;
}

