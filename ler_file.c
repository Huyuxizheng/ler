#include "ler_file.h"
#include "ler_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper to read file content */
static char* read_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        ler_err("Failed to open file: %s\n", path);
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (size < 0) {
        fclose(f);
        return NULL;
    }

    char* content = (char*)ler_malloc(size + 1);
    if (!content) {
        fclose(f);
        return NULL;
    }
    
    size_t read_size = fread(content, 1, size, f);
    content[read_size] = '\0';
    fclose(f);
    
    return content;
}

/* Helper to extract content between tags */
static char* extract_tag_content(const char* source, const char* tag_start, const char* tag_end) {
    const char* start = strstr(source, tag_start);
    if (!start) return NULL;
    
    start += strlen(tag_start);
    const char* end = strstr(start, tag_end);
    if (!end) return NULL;
    
    size_t len = end - start;
    char* content = (char*)ler_malloc(len + 1);
    if (content) {
        memcpy(content, start, len);
        content[len] = '\0';
    }
    return content;
}

ler_file_content_t* ler_file_parse(const char* file_path) {
    char* raw_content = read_file(file_path);
    if (!raw_content) return NULL;
    
    ler_file_content_t* result = (ler_file_content_t*)ler_malloc(sizeof(ler_file_content_t));
    if (!result) {
        ler_free(raw_content);
        return NULL;
    }
    memset(result, 0, sizeof(ler_file_content_t));
    
    /* Extract <template>...</template> */
    result->xml_content = extract_tag_content(raw_content, "<template>", "</template>");
    
    /* Extract <script>...</script> */
    result->lua_content = extract_tag_content(raw_content, "<script>", "</script>");
    
    ler_free(raw_content);
    return result;
}

void ler_file_free_content(ler_file_content_t* content) {
    if (!content) return;
    if (content->xml_content) ler_free(content->xml_content);
    if (content->lua_content) ler_free(content->lua_content);
    ler_free(content);
}
