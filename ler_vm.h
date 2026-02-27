#ifndef LER_VM_H
#define LER_VM_H

#include "ler_dom.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

typedef struct {
    int ref;
    lua_State *L;
} lua_val_t;

typedef struct _ler_vm_t ler_vm_t;

struct _ler_vm_t{
    ler_dom_node_t* dom;
    lua_val_t* env;
    ler_vm_t *sub_vm;
    ler_vm_t *next;
};

ler_vm_t* ler_vm_create(lua_State *L);
void ler_vm_destroy(ler_vm_t* vm);

#endif
