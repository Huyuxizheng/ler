#include "ler.h"
#include "lvgl.h"

ler_dom_node_t* ler_dom_bar_create(ler_dom_node_t *node){
    node->obj = lv_bar_create(node->parent->obj);
    
    // Force styles to ensure visibility on any background
    // lv_obj_set_style_bg_color(node->obj, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
    // lv_obj_set_style_bg_opa(node->obj, LV_OPA_COVER, LV_PART_MAIN);
    
    // lv_obj_set_style_bg_color(node->obj, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    // lv_obj_set_style_bg_opa(node->obj, LV_OPA_COVER, LV_PART_INDICATOR);
    // ler_log("Created bar at (%d, %d) with value %d.\n", lv_obj_get_x(node->obj), lv_obj_get_y(node->obj), lv_bar_get_value(node->obj));
    return node;
}
ler_dom_node_attr_t* ler_dom_bar_get_attr(ler_dom_node_t *node, const char* name){
    return ler_dom_default_type.get_attr(node,name);
}
uint8_t ler_dom_bar_set_attr(ler_dom_node_t *node,const char* name,const char *attr_str){
    if(strcmp(name,"value") == 0){
        int32_t val = atoi(attr_str);
        lv_bar_set_value(node->obj, val, LV_ANIM_OFF);
        return 0;
    }
    return ler_dom_default_type.set_attr(node,name,attr_str);
}
void ler_dom_bar_remove(ler_dom_node_t *node){
    lv_obj_del(node->obj);
}

const ler_dom_node_type_t ler_dom_bar_type = {
    /*.create = */  ler_dom_bar_create,
    /*.get_attr = */ler_dom_bar_get_attr, 
    /*.set_attr = */ler_dom_bar_set_attr,
    /*.remove = */  ler_dom_bar_remove,
};
