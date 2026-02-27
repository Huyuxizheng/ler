#include "ler_vm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void copy_global(lua_State *L, const char *name) {
    lua_getglobal(L, name);
    lua_setfield(L, -2, name);
}

ler_vm_t* ler_vm_create(lua_State *L){
    ler_vm_t* vm = (ler_vm_t*)malloc(sizeof(ler_vm_t));
    if(!vm) return NULL;
    memset(vm, 0, sizeof(ler_vm_t));

    vm->env = (lua_val_t*)malloc(sizeof(lua_val_t));
    if(!vm->env) {
        free(vm);
        return NULL;
    }
    vm->env->L = L;

    lua_newtable(L); /* 创建新的环境表 */

    /* 初始化 _global 指向 _G */
    lua_pushglobaltable(L);
    lua_setfield(L, -2, "_global");

    /* 复制标准库和常用函数 */
    copy_global(L, "print");
    copy_global(L, "math");
    copy_global(L, "string");
    copy_global(L, "table");
    copy_global(L, "type");
    copy_global(L, "tostring");
    copy_global(L, "tonumber");
    copy_global(L, "pairs");
    copy_global(L, "ipairs");
    copy_global(L, "next");
    copy_global(L, "error");

    /* 将环境表存入注册表，并保存引用 */
    vm->env->ref = luaL_ref(L, LUA_REGISTRYINDEX);

    return vm;
}

void ler_vm_destroy(ler_vm_t* vm) {
    if (!vm) return;
    if (vm->env) {
        luaL_unref(vm->env->L, LUA_REGISTRYINDEX, vm->env->ref);
        free(vm->env);
    }
    free(vm);
}

int ler_vm_exec(ler_vm_t* vm, const char* script) {
    if (!vm || !vm->env || !script) return -1;
    lua_State *L = vm->env->L;
    
    int status = luaL_loadstring(L, script);
    if (status != LUA_OK) {
        const char *msg = lua_tostring(L, -1);
        printf("Lua load error: %s\n", msg);
        lua_pop(L, 1);
        return status;
    }
    
    // Set _ENV for the chunk
    lua_rawgeti(L, LUA_REGISTRYINDEX, vm->env->ref); // Push env table
    lua_setupvalue(L, -2, 1); // Set _ENV (upvalue 1)
    
    status = lua_pcall(L, 0, 0, 0);
    if (status != LUA_OK) {
        const char *msg = lua_tostring(L, -1);
        printf("Lua exec error: %s\n", msg);
        lua_pop(L, 1);
        return status;
    }
    
    return 0;
}
