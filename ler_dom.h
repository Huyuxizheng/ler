#ifndef LER_DOM_H
#define LER_DOM_H
#include <stdint.h>
#include <stdbool.h>


struct _ler_dom_node_t;
typedef struct _ler_dom_node_t ler_dom_node_t;//向前声明   
typedef enum {
    LER_DOM_NODE_ATTR_TYPE_UNKNOWN = 0,
    LER_DOM_NODE_ATTR_TYPE_ROOT = 1,
    LER_DOM_NODE_ATTR_TYPE_INT = 2,
    LER_DOM_NODE_ATTR_TYPE_FLOAT = 3,
    LER_DOM_NODE_ATTR_TYPE_STRING = 4,
    LER_DOM_NODE_ATTR_TYPE_BOOL = 5
}ler_dom_node_attr_type_e;

typedef struct {
    ler_dom_node_attr_type_e type;
    union {
        int32_t i;
        float f;
        char* s;
        bool b;
    } value;
} ler_dom_node_attr_t;

typedef struct {
    ler_dom_node_t*         (*create)(ler_dom_node_t *node);
    ler_dom_node_attr_t*    (*get_attr)(ler_dom_node_t *node,const char* name);
    uint8_t                 (*set_attr)(ler_dom_node_t *node,const char* name,const char *attr_str);
    void                    (*remove)(ler_dom_node_t *node);
} ler_dom_node_type_t;

typedef struct _ler_dom_node_t{
    struct _ler_dom_node_t *parent;
    struct _ler_dom_node_t *child;
    struct _ler_dom_node_t *next;
    const ler_dom_node_type_t* type;
    void* obj;
} ler_dom_node_t;

ler_dom_node_t* ler_dom_create_form_xml(const char* xml, void *root_obj);
ler_dom_node_t* ler_dom_create_node_form_xml(const char* xml_name);
ler_dom_node_attr_t* ler_dom_create_attr_form_xml(const char* xml_value);

#endif
