#ifndef LER_FILE_H
#define LER_FILE_H

#include <stdint.h>

/* Structure to hold separated content from .ler file */
typedef struct {
    char* xml_content;
    char* lua_content;
} ler_file_content_t;

/**
 * Parse a .ler file content and separate XML (template) and Lua (script) parts.
 * 
 * @param file_path Path to the .ler file
 * @return Pointer to ler_file_content_t containing allocated strings, or NULL on failure.
 *         Caller must free the returned structure and its members using ler_file_free_content().
 */
ler_file_content_t* ler_file_parse(const char* file_path);

/**
 * Free the memory allocated for ler_file_content_t.
 * 
 * @param content Pointer to the structure to free.
 */
void ler_file_free_content(ler_file_content_t* content);

#endif
