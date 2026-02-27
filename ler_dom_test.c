#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"
#include "lv_drivers/sdl/sdl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include "ler.h"

/* Helper to print DOM tree */
void print_dom_tree(ler_dom_node_t* node, int depth) {
    if (!node) return;
    
    for(int i=0; i<depth; i++) printf("  ");
    
    if (node->type == NULL) {
        printf("Root Node (obj: %p)\n", node->obj);
    } else {
        const char* type_name = "Unknown";
        if (node->type == &ler_dom_text_type) type_name = "Text";
        else if (node->type == &ler_dom_button_type) type_name = "Button";
        else if (node->type == &ler_dom_bar_type) type_name = "Bar";
        else if (node->type == &ler_dom_view_type) type_name = "View";
        else if (node->type == &ler_dom_default_type) type_name = "Default";
        
        printf("Node Type: %s (obj: %p)\n", type_name, node->obj);
    }

    ler_dom_node_t* child = node->child;
    while(child) {
        print_dom_tree(child, depth + 1);
        child = child->next;
    }
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void)
{
  /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  sdl_init();

  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[480 * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, NULL, 480 * 100);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = sdl_display_flush;
  disp_drv.hor_res = 480;
  disp_drv.ver_res = 320;

  lv_disp_t * disp = lv_disp_drv_register(&disp_drv);

  lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);

  lv_group_t * g = lv_group_create();
  lv_group_set_default(g);

  /* Add the mouse as input device
   * Use the 'mouse' driver which reads the PC's mouse*/
  static lv_indev_drv_t indev_drv_1;
  lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
  indev_drv_1.type = LV_INDEV_TYPE_POINTER;

  /*This function will be called periodically (by the library) to get the mouse position and state*/
  indev_drv_1.read_cb = sdl_mouse_read;
  lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

  static lv_indev_drv_t indev_drv_2;
  lv_indev_drv_init(&indev_drv_2); /*Basic initialization*/
  indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
  indev_drv_2.read_cb = sdl_keyboard_read;
  lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
  lv_indev_set_group(kb_indev, g);

  static lv_indev_drv_t indev_drv_3;
  lv_indev_drv_init(&indev_drv_3); /*Basic initialization*/
  indev_drv_3.type = LV_INDEV_TYPE_ENCODER;
  indev_drv_3.read_cb = sdl_mousewheel_read;
  lv_indev_t * enc_indev = lv_indev_drv_register(&indev_drv_3);
  lv_indev_set_group(enc_indev, g);

  /*Set a cursor for the mouse*/
  LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
  lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
  lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/
}

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init();

  printf("LVGL initialized with SDL driver.\n");

  lv_obj_t *root_obj = lv_scr_act();
  printf("Root object created: %p\n", root_obj);

  const char* xml = 
      "<view>"
      "  <button text=\"Click Me\" x=\"50\" y=\"50\" />"
      "  <view x=\"50\" y=\"100\" width=\"200\" height=\"100\">"
      "      <text text=\"Inside View\" x=\"10\" y=\"10\" />"
      "  </view>"
      "  <bar value=\"70\" x=\"50\" y=\"250\" width=\"200\" height=\"20\" />"
      "</view>";

  printf("Parsing XML:\n%s\n", xml);
  
  ler_dom_node_t* dom_root = ler_dom_create_form_xml(xml, root_obj);
  
  if (dom_root) {
      printf("\nDOM Tree constructed successfully:\n");
      print_dom_tree(dom_root, 0);
  } else {
      printf("Failed to create DOM.\n");
  }
    //lv_demo_widgets();
  while(1) {
      /* Periodically call the lv_task handler.
       * It could be done in a timer interrupt or an OS task too.*/
      lv_timer_handler();
      usleep(5 * 1000);
  }

  return 0;
}
