#include "ler.h"
#include "lvgl.h"

ler_dom_node_t* ler_dom_button_create(ler_dom_node_t *node){
    node->obj = lv_btn_create(node->parent->obj);
    return node;
}
ler_dom_node_attr_t* ler_dom_button_get_attr(ler_dom_node_t *node,const char* name){
    return ler_dom_default_type.get_attr(node,name);
}
uint8_t ler_dom_button_set_attr(ler_dom_node_t *node,const char* name,const char *attr_str){
    if(strcmp(name,"text") == 0){
        lv_label_set_text(node->obj,attr_str);
        return 1;
    }
    return ler_dom_default_type.set_attr(node,name,attr_str);
}
void ler_dom_button_remove(ler_dom_node_t *node){
    lv_obj_del(node->obj);
}

const ler_dom_node_type_t ler_dom_button_type = {
    /*.create = */  ler_dom_button_create,
    /*.get_attr = */ler_dom_button_get_attr, 
    /*.set_attr = */ler_dom_button_set_attr,
    /*.remove = */  ler_dom_button_remove,
};
