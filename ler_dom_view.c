#include "ler.h"
#include "lvgl.h"

ler_dom_node_t* ler_dom_view_create(ler_dom_node_t *node){
    node->obj = lv_obj_create(node->parent->obj);
    
    // 移除边框、背景等样式，只作为编组容器
    // 注意：必须先移除样式，再设置尺寸，因为 lv_obj_remove_style_all 会清除所有样式属性包括尺寸
    lv_obj_remove_style_all(node->obj);
    // 默认情况下自动调整尺寸 
    lv_obj_set_width(node->obj, LV_SIZE_CONTENT);
    lv_obj_set_height(node->obj, LV_SIZE_CONTENT);
    return node;
}

ler_dom_node_attr_t* ler_dom_view_get_attr(ler_dom_node_t *node,const char* name){
    return ler_dom_default_type.get_attr(node,name);
}

uint8_t ler_dom_view_set_attr(ler_dom_node_t *node,const char* name,const char *attr_str){
    // 可以在这里添加 view 特有的属性处理
    // 例如处理 width/height 覆盖默认的 100%
    return ler_dom_default_type.set_attr(node,name,attr_str);
}

void ler_dom_view_remove(ler_dom_node_t *node){
    lv_obj_del(node->obj);
}

const ler_dom_node_type_t ler_dom_view_type = {
    /*.create = */  ler_dom_view_create,
    /*.get_attr = */ler_dom_view_get_attr, 
    /*.set_attr = */ler_dom_view_set_attr,
    /*.remove = */  ler_dom_view_remove,
};
