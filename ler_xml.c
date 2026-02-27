#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include "ler.h"


/* 简单的HTML实体解码表（仅做示例，可按需扩展） */
static const struct {
    const char* entity;
    char        ch;
} g_decode_map[] = {
    {"lt;",   '<'},
    {"gt;",   '>'},
    {"amp;",  '&'},
    {"quot;", '"'},
    {"apos;", '\''},
    {NULL, 0}
};

/* 原地将一段字符串做实体解码，返回解码后长度 */
static int ler_xml_decode_inplace(char* src, int len)
{
    char *dst = src;
    for (int i = 0; i < len; ) {
        if (src[i] == '&') {
            int found = 0;
            for (int k = 0; g_decode_map[k].entity; ++k) {
                int elen = (int)strlen(g_decode_map[k].entity);
                if (i + elen < len && memcmp(&src[i+1], g_decode_map[k].entity, elen) == 0) {
                    *dst++ = g_decode_map[k].ch;
                    i += elen + 1; /* 跳过 '&' 和实体 */
                    found = 1;
                    break;
                }
            }
            if (!found) {
                /* 未识别实体，原样保留 */
                *dst++ = src[i++];
            }
        } else {
            *dst++ = src[i++];
        }
    }
    int newlen = (int)(dst - src);
    src[newlen] = '\0';
    return newlen;
}

/* 单次遍历解析XML，遇到开始标签/属性/结束标签/文本内容时回调 */
uint8_t ler_xml_parse(const char* xml, ler_xml_callback_t cb)
{
    if (!xml || !cb) return 1;

    const char *p = xml;
    while (*p) {
        /* 检查是否有文本内容 */
        const char *text_start = p;
        while (*p && *p != '<') ++p;
        
        /* 如果有非空白的文本内容，进行回调 */
        if (p > text_start) {
            /* 创建文本内容的副本 */
            int text_len = (int)(p - text_start);
            char *text_buf = (char*)malloc(text_len + 1);
            if (!text_buf) return 4;
            memcpy(text_buf, text_start, text_len);
            text_buf[text_len] = '\0';
            
            /* 解码HTML实体 */
            ler_xml_decode_inplace(text_buf, text_len);
            
            /* 检查是否全是空白字符 */
            int all_whitespace = 1;
            for (int i = 0; i < (int)strlen(text_buf); ++i) {
                if (text_buf[i] != ' ' && text_buf[i] != '\t' && text_buf[i] != '\r' && text_buf[i] != '\n') {
                    all_whitespace = 0;
                    break;
                }
            }
            
            /* 只回调非空白文本 */
            if (!all_whitespace) {
                cb(3, NULL, text_buf);
            }
            
            free(text_buf);
        }
        
        if (!*p) break; // 确保指针有效
        
        const char *tag_start = ++p;
        /* 找标签结束 '>' */
        const char *tag_end = strchr(tag_start, '>');
        if (!tag_end) return 3;

        int tag_len = (int)(tag_end - tag_start);
        char *buf = (char*)malloc(tag_len + 1);
        if (!buf) return 4;
        memcpy(buf, tag_start, tag_len);
        buf[tag_len] = '\0';

        /* 处理结束标签 </xxx> */
        if (buf[0] == '/') {
            ler_xml_decode_inplace(buf + 1, tag_len - 1);
            cb(2, buf + 1, NULL);
            free(buf);
            p = tag_end + 1;
            continue;
        }

        /* 处理开始标签 <xxx ...> */
        int is_self_closing = 0;
        if (*(tag_end - 1) == '/') {
            is_self_closing = 1;
        }

        char *sp = strchr(buf, ' ');
        char *tag_name = buf;
        
        if (!sp) {
             /* Handle case like <foo/> where no space exists */
             size_t len = strlen(tag_name);
             if (len > 0 && tag_name[len-1] == '/') {
                 tag_name[len-1] = '\0';
             }
        } else {
             *sp++ = '\0';
        }
        
        ler_xml_decode_inplace(tag_name, (int)strlen(tag_name));
        cb(0, tag_name, NULL);

        /* 解析属性 */
        while (sp && *sp) {
            /* 跳过空白 */
            while (*sp == ' ' || *sp == '\t') ++sp;
            if (!*sp) break;
            
            /* 检查是否是自闭合标签结束 */
            if (*sp == '/' || *sp == '>') break;
            
            /* 查找属性名结束位置 */
            char *attr_end = sp;
            while (*attr_end && *attr_end != '=' && *attr_end != ' ' && *attr_end != '\t' && *attr_end != '/' && *attr_end != '>') {
                ++attr_end;
            }
            
            /* 保存当前属性名结束位置 */
            char attr_end_char = *attr_end;
            *attr_end = '\0';
            
            /* 解码属性名 */
            char *attr_name = sp; // 保存属性名的起始地址
            ler_xml_decode_inplace(attr_name, (int)strlen(attr_name));
            
            if (attr_end_char == '=') {
                /* 有等号和值的属性 */
                sp = attr_end + 1;
                /* 属性值可能被 " 或 ' 包围 */
                char quote = *sp;
                if (quote == '"' || quote == '\'') {
                    ++sp;
                    
                    /* 查找值的结束位置，考虑嵌套的引号和Vue的{{ }}格式 */
                    char *val_end = sp;
                    int brace_count = 0;
                    while (*val_end) {
                        /* 处理Vue的{{ }}插值表达式 */
                        if (*val_end == '{' && *(val_end + 1) == '{') {
                            brace_count++;
                            val_end += 2;
                            continue;
                        }
                        if (*val_end == '}' && *(val_end + 1) == '}' && brace_count > 0) {
                            brace_count--;
                            val_end += 2;
                            continue;
                        }
                        
                        /* 如果是引号且不在Vue模板内，则是值的结束 */
                        if (*val_end == quote && brace_count == 0) {
                            break;
                        }
                        
                        val_end++;
                    }
                    
                    if (!*val_end) return 6; // 未找到匹配的引号
                    
                    *val_end = '\0';
                    ler_xml_decode_inplace(sp, (int)strlen(sp));
                    cb(1, attr_name, sp);
                    sp = val_end + 1;
                } else {
                    /* 非引号包围的值（简化处理） */
                    char *val_end = sp;
                    while (*val_end && *val_end != ' ' && *val_end != '\t' && *val_end != '/' && *val_end != '>') {
                        ++val_end;
                    }
                    char val_end_char = *val_end;
                    *val_end = '\0';
                    ler_xml_decode_inplace(sp, (int)strlen(sp));
                    cb(1, attr_name, sp);
                    *val_end = val_end_char; /* 恢复原字符 */
                    sp = val_end;
                }
            } else {
                /* 没有值的属性 */
                cb(1, attr_name, "");
                *attr_end = attr_end_char; /* 恢复原字符 */
                sp = attr_end;
            }
        }

        if (is_self_closing) {
            cb(2, tag_name, NULL);
        }

        free(buf);
        p = tag_end + 1;
    }
    return 0;
}







