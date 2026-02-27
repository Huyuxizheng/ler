#include "ler.h"
#include "lvgl.h"

ler_dom_node_t* ler_dom_default_create(ler_dom_node_t *node){
    node->obj = lv_obj_create(node->parent->obj);
    lv_obj_set_size(node->obj, LV_PCT(100), LV_PCT(100)); // Default to fill parent
    return node;
}
ler_dom_node_attr_t* ler_dom_default_get_attr(ler_dom_node_t *node,const char* name){
    return node->type->get_attr(node,name);
}
uint8_t ler_dom_default_set_attr(ler_dom_node_t *node,const char* name,const char *attr_str){
    int32_t i;
    float f;
    bool b;
    
    if(strcmp(name,"x") == 0){
        i = atoi(attr_str);
        ler_log("%s = %d",name,i);
        lv_obj_set_x(node->obj, i);
        return 0;
    }
    if(strcmp(name,"y") == 0){
        i = atoi(attr_str);
        ler_log("%s = %d",name,i);
        lv_obj_set_y(node->obj, i);
        return 0;
    }
    if(strcmp(name,"width") == 0){
        i = atoi(attr_str);
        ler_log("%s = %d",name,i);
        lv_obj_set_width(node->obj, i);
        return 0;
    }
    if(strcmp(name,"height") == 0){
        i = atoi(attr_str);
        ler_log("%s = %d",name,i);
        lv_obj_set_height(node->obj, i);
        return 0;
    }
    return 1;
}
void ler_dom_default_remove(ler_dom_node_t *node){
    lv_obj_del(node->obj);
}

const ler_dom_node_type_t ler_dom_default_type = {
    /*.create = */  ler_dom_default_create,
    /*.get_attr = */ler_dom_default_get_attr, 
    /*.set_attr = */ler_dom_default_set_attr,
    /*.remove = */  ler_dom_default_remove,
};