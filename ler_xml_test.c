#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include "ler.h"

/* XML 解析回调函数 */
static void xml_test_callback(int type, const char* name, const char* value) {
    switch (type) {
        case 0: /* 开始标签 */
            printf("Start tag: <%s>\n", name);
            break;
        case 1: /* 属性 */
            printf("  Attribute: %s=\"%s\"\n", name, value);
            break;
        case 2: /* 结束标签 */
            printf("End tag: </%s>\n", name);
            break;
        case 3: /* 文本内容 */
            printf("Text content: %s\n", value);
            break;
    }
}

/* 测试单个 XML 字符串 */
static void run_xml_test(const char* test_name, const char* xml) {
    printf("\n=== Test: %s ===\n", test_name);
    printf("XML content: %s\n", xml);
    printf("Parsing result:\n");
    
    uint8_t result = ler_xml_parse(xml, xml_test_callback);
    
    if (result == 0) {
        printf("OK: Test passed!\n");
    } else {
        printf("ERROR: Test failed, error code: %u\n", result);
    }
}

/* 主函数 */
int main() {
    printf("Starting XML parser tests...\n\n");
    
    /* Test case 1: Simple tags */
    run_xml_test("Simple tags", "<root><node>text</node></root>");
    
    /* Test case 2: Tags with attributes */
    run_xml_test("Tags with attributes", "<person name=\"John\" age=\"30\" age1><info active=\"true\"/></person>");
    
    /* Test case 3: Vue interpolation */
    run_xml_test("Vue interpolation", "<div class=\"item {{ isActive ? 'active' : '' }}\" v-if=\"showItem\">\n  <span>{{ userName }}</span>\n</div>");
    
    /* Test case 4: HTML entities */
    run_xml_test("HTML entities", "<message>Hello &lt;World&gt; &amp; welcome!</message>");
    
    /* Test case 5: Nested tags */
    run_xml_test("Nested tags", "<root><level1><level2>deep<level3></level3> content</level2></level1></root>");
    
    /* Test case 6: Attributes with quotes */
    run_xml_test("Attributes with quotes", "<element attr1=\"simple value\" attr2=\"test\" />");
    
    /* Test case 7: Complex Vue interpolation */
    run_xml_test("Complex Vue interpolation", "<button @click=\"handleClick({{ item.id }})\" :disabled=\"{{ loading || !hasPermission }}\">\n  {{ buttonText || 'Submit' }}\n</button>");
    
    printf("\nAll tests completed!\n");
    return 0;
}





