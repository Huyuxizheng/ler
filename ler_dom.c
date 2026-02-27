#include "ler.h"


// 栈结构，用于解析XML时暂存节点
typedef struct {
    uint32_t top;
    ler_dom_node_t* dom_stack[128];
} dom_stack_t;
dom_stack_t *dom_stack;
uint8_t dom_stack_init()
{
    dom_stack = (dom_stack_t*)ler_malloc(sizeof(dom_stack_t));
    if (!dom_stack) return 1;
    dom_stack->top = 0;
    dom_stack->dom_stack[0] = NULL;
    return 0;
}
void dom_stack_push(ler_dom_node_t* node)
{
    if (!node) return;
    dom_stack->dom_stack[++dom_stack->top] = node;
}
ler_dom_node_t* dom_stack_pop()
{
    if (dom_stack->top <= 0) return NULL;
    return dom_stack->dom_stack[dom_stack->top--];  
}
ler_dom_node_t* dom_stack_get_top()
{
    if (dom_stack->top <= 0) return NULL;
    return dom_stack->dom_stack[dom_stack->top];  
}
void dom_stack_uninit()
{
    ler_free(dom_stack);
}

/* 新增：单独函数实现挂接逻辑 */
static void ler_dom_attach_to_parent(ler_dom_node_t* node)
{
    ler_dom_node_t* parent = dom_stack_get_top();
    if (!parent) return;

    node->parent = parent;
    node->next = NULL;          /* 新节点作为最后一个孩子 */
    if (!parent->child) {       /* 父节点尚无孩子 */
        parent->child = node;
    } else {                    /* 已有孩子，遍历到链表尾 */
        ler_dom_node_t* sibling = parent->child;
        while (sibling->next) {
            sibling = sibling->next;
        }
        sibling->next = node;
    }
}

ler_dom_node_t* ler_dom_create_node_form_xml(const char* xml_name)
{
    if (!xml_name) return NULL;
    ler_dom_node_t* node = (ler_dom_node_t*)ler_malloc(sizeof(ler_dom_node_t));
    if (!node) return NULL;
    memset(node, 0, sizeof(ler_dom_node_t));
    if (strcmp(xml_name,"text") == 0) {
        node->type = &ler_dom_text_type;
    } else if (strcmp(xml_name,"button") == 0) {
        node->type = &ler_dom_button_type;
    } else if (strcmp(xml_name,"bar") == 0) {
        node->type = &ler_dom_bar_type;
    } else if (strcmp(xml_name,"view") == 0) {
        node->type = &ler_dom_view_type;
    } else {
        node->type = &ler_dom_default_type;
    }
    
    /* 将新节点挂到父节点的孩子链表尾部 */
    ler_dom_attach_to_parent(node);

    node->type->create(node);
    return node;
}
ler_dom_node_attr_t* ler_dom_create_attr_form_xml(const char* xml_value)
{
    if (!xml_value) return NULL;
    ler_dom_node_attr_t* attr = (ler_dom_node_attr_t*)ler_malloc(sizeof(ler_dom_node_attr_t));
    if (!attr) return NULL;
    memset(attr, 0, sizeof(ler_dom_node_attr_t));
    attr->type = LER_DOM_NODE_ATTR_TYPE_STRING;
    attr->value.s = ler_strdup(xml_value);
    return attr;
}
static void ler_dom_create_form_xml_cb(int type, const char* name, const char* _value)
{
    ler_dom_node_t* node = NULL;
     switch (type) {
     case 0: /* 开始标签 */
        if (!name) return;
         printf("Start tag: <%s>\n", name);
         node = ler_dom_create_node_form_xml(name);
         if (!node) return;
         dom_stack_push(node);
         break; 
     case 1: /* 属性 */
        if (!name) return;
         printf("  Attribute: %s=\"%s\"\n", name, _value);
         node = dom_stack_get_top();
         if (!node) return;
         switch (name[0])
         {
         case '@'://创建事件回调处理
             node->type->set_attr(node,name+1,_value);
             break;
         case ':'://创建观察者对象
             node->type->set_attr(node,name+1,_value);
             break;
         default://直接赋值
             node->type->set_attr(node,name,_value);
             break;
         }
         break;
     case 2: /* 结束标签 */
        if (!name) return;
         printf("End tag: </%s>\n", name);
         node = dom_stack_pop();
         if (!node) return;
         break;
     case 3: /* 文本内容 */
         printf("Text content: %s\n", _value);
         node = dom_stack_get_top();
         if (!node) return;
         node->type->set_attr(node,"text",_value);
         break;
    }
}
ler_dom_node_t* ler_dom_create_form_xml(const char* xml,void *root_obj)
{
    ler_dom_node_t* root = NULL;
    if (!xml) return NULL;
    if (dom_stack_init()) return NULL;
    root = (ler_dom_node_t*)ler_malloc(sizeof(ler_dom_node_t));
    if (!root) return NULL;
    memset(root, 0, sizeof(ler_dom_node_t));
    root->type = NULL;
    root->obj = root_obj;

    dom_stack_push(root);
    ler_xml_parse(xml, (ler_xml_callback_t)ler_dom_create_form_xml_cb);

    if (dom_stack_get_top() != root) 
    {
        ler_log("dom_stack_get_top() != root (top=%p, root=%p)\n", dom_stack_get_top(), root);
    }
    dom_stack_uninit();
    return root;
}


