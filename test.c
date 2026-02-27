#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>  // 用于getcwd函数
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

// 获取并打印当前工作目录
void print_current_directory() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        printf("Error getting current directory\n");
    }
}

// 尝试多种路径加载Lua文件
int load_lua_file(lua_State* L, const char* file_path) {
    // 打印当前工作目录
    print_current_directory();
    
    // 尝试的路径列表
    const char* paths[] = {
        file_path,                    // 直接路径
        ".\\%s",                    // 当前目录
        "..\\%s",                   // 上一级目录
        "E:\\code\\lerf\\%s",    // 绝对路径
        "build\\windows\\x64\\release\\%s" // 可能的输出目录
    };
    
    for (int i = 0; i < sizeof(paths)/sizeof(paths[0]); i++) {
        char full_path[1024];
        if (i == 0) {
            // 直接使用原始路径
            strcpy(full_path, file_path);
        } else {
            // 使用格式化路径
            snprintf(full_path, sizeof(full_path), paths[i], file_path);
        }
        
        printf("Trying to load: %s\n", full_path);
        int status = luaL_loadfile(L, full_path);
        
        if (status == LUA_OK) {
            printf("Successfully loaded: %s\n", full_path);
            return 1;
        }
        
        // 清除栈上的错误信息
        lua_pop(L, 1);
    }
    
    // 所有路径都失败了
    return 0;
}

int main(int argc, char** argv) {
    // 检查命令行参数
    if (argc < 2) {
        printf("Usage: %s <lua_file.lua>\n", argv[0]);
        return 1;
    }
    
    const char* lua_file = argv[1];
    
    // 初始化Lua状态
    lua_State* L = luaL_newstate();
    if (L == NULL) {
        printf("Error: Cannot create Lua state\n");
        return 1;
    }
    
    // 打开标准Lua库
    luaL_openlibs(L);
    
    // 加载Lua文件
    printf("Loading Lua file: %s\n", lua_file);
    
    if (!load_lua_file(L, lua_file)) {
        const char* error_msg = lua_tostring(L, -1);
        printf("Error loading file: %s\n", error_msg);
        lua_close(L);
        return 1;
    }
    
    // 执行Lua文件
    printf("Executing Lua file...\n");
    int status = lua_pcall(L, 0, LUA_MULTRET, 0);
    
    if (status != LUA_OK) {
        const char* error_msg = lua_tostring(L, -1);
        printf("Error executing file: %s\n", error_msg);
        lua_close(L);
        return 1;
    }
    
    printf("Lua file executed successfully!\n");
    
    // 清理Lua状态
    lua_close(L);
    
    return 0;
}