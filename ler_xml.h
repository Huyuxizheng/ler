#ifndef LER_XML_H
#define LER_XML_H

#include <stdint.h>

/* 回调函数原型：遇到开始标签/属性/结束标签/文本内容时调用
 * type: 0=开始标签 1=属性 2=结束标签 3=文本内容
 * name: 标签名或属性名（文本内容时为NULL）
 * value: 属性值或文本内容
 */
typedef void (*ler_xml_callback_t)(int type, const char* name, const char* value);

extern uint8_t ler_xml_parse(const char* xml, ler_xml_callback_t cb);

#endif
