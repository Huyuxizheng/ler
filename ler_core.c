#include "ler.h"
#include "lvgl.h"


//从文件系统加载
void ler_laod_app_fs()
{
    ler_log("ler_init");
}

//从内存加载
// 初始化环境
void ler_laod_app_for_buf_step1_init(void)
{
    ler_log("ler_laod_app_for_buf");
}
//加载所有lua脚本
void ler_laod_app_for_buf_step2_lua(char *path,char *lua_buf,size_t size)
{
    ler_log("ler_laod_app_for_buf %s",path);
}
//加载所有xml
void ler_laod_app_for_buf_step2_xml(char *path,char *xml_buf,size_t size)
{
    ler_log("ler_laod_app_for_buf %s",path);
}
//创建lua对象
void ler_laod_app_for_buf_step3_load_lua(void)
{
    ler_log("ler_laod_app_for_buf");
} 
//创建vm 并渲染
void ler_laod_app_for_buf_step4_create_vm_and_render(lv_obj_t *root_obj)
{
    ler_log("ler_laod_app_for_buf");
}