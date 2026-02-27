#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"
#include "lv_drivers/sdl/sdl.h"
#include "ler.h"
#include "ler_file.h"
#include "ler_vm.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/**
 * 初始化 LVGL 图形库的硬件抽象层 (HAL)
 */
static void hal_init(void)
{
  /* 使用“monitor”驱动，在 PC 显示器上创建窗口来模拟显示屏 */
  sdl_init();

  /* 创建显示缓冲区 */
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[480 * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, NULL, 480 * 100);

  /* 创建显示设备 */
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv); /* 基础初始化 */
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = sdl_display_flush;
  disp_drv.hor_res = 480;
  disp_drv.ver_res = 320;

  lv_disp_t * disp = lv_disp_drv_register(&disp_drv);

  lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);

  lv_group_t * g = lv_group_create();
  lv_group_set_default(g);

  /* 添加鼠标作为输入设备
   * 使用“mouse”驱动读取 PC 鼠标 */
  static lv_indev_drv_t indev_drv_1;
  lv_indev_drv_init(&indev_drv_1); /* 基础初始化 */
  indev_drv_1.type = LV_INDEV_TYPE_POINTER;

  /* 该函数将被库定期调用以获取鼠标位置和状态 */
  indev_drv_1.read_cb = sdl_mouse_read;
  lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

  static lv_indev_drv_t indev_drv_2;
  lv_indev_drv_init(&indev_drv_2); /* 基础初始化 */
  indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
  indev_drv_2.read_cb = sdl_keyboard_read;
  lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
  lv_indev_set_group(kb_indev, g);

  static lv_indev_drv_t indev_drv_3;
  lv_indev_drv_init(&indev_drv_3); /* 基础初始化 */
  indev_drv_3.type = LV_INDEV_TYPE_ENCODER;
  indev_drv_3.read_cb = sdl_mousewheel_read;
  lv_indev_t * enc_indev = lv_indev_drv_register(&indev_drv_3);
  lv_indev_set_group(enc_indev, g);

  /* 设置鼠标光标 */
  LV_IMG_DECLARE(mouse_cursor_icon); /* 声明图像文件 */
  lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /* 为光标创建图像对象 */
  lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /* 设置图像源 */
  lv_indev_set_cursor(mouse_indev, cursor_obj);             /* 将图像对象连接到驱动程序 */
}

int main(int argc, char **argv)
{
  (void)argc; /* 未使用 */
  (void)argv; /* 未使用 */

  /* 初始化 LVGL */
  lv_init();

  /* 初始化 LVGL 的 HAL（显示、输入设备、时钟） */
  hal_init();

  printf("LVGL initialized with SDL driver.\n");

  lv_obj_t *root_obj = lv_scr_act();
  printf("Root object created: %p\n", root_obj);

  /* 初始化 Lua 和 VM */
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  ler_vm_t *vm = ler_vm_create(L);
  if (!vm) {
      printf("Failed to create VM.\n");
      return -1;
  }

  /* 解析 .ler 文件 */
  const char* ler_path = "e:/code/ler/test_app/app1/index.ler";
  printf("Loading .ler file from: %s\n", ler_path);
  
  ler_file_content_t* content = ler_file_parse(ler_path);
  
  if (content) {
      if (content->xml_content) {
          printf("XML Content:\n%s\n", content->xml_content);
          printf("Root object size: (%d, %d)\n", lv_obj_get_width(root_obj), lv_obj_get_height(root_obj));
          ler_dom_node_t* dom_root = ler_dom_create_form_xml(content->xml_content, root_obj);
          if (dom_root) {
              printf("DOM Tree created successfully.\n");
              vm->dom = dom_root;
          } else {
              printf("Failed to create DOM from XML.\n");
          }
      } else {
          printf("No XML content found in .ler file.\n");
      }

      if (content->lua_content) {
          printf("\nLua Content:\n%s\n", content->lua_content);
          ler_vm_exec(vm, content->lua_content);
      } else {
          printf("\nNo Lua content found in .ler file.\n");
      }
      
      ler_file_free_content(content);
  } else {
      printf("Failed to parse .ler file.\n");
  }

  while(1) {
      /* 定期调用 lv_task 处理程序。
       * 也可以在定时器中断或操作系统任务中完成。*/
      lv_timer_handler();
      usleep(5 * 1000);
  }

  return 0;
}
