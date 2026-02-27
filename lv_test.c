#include "lvgl.h"
#include "lv_drivers/sdl/sdl.h"

int main(void)
{
    lv_init();
    sdl_init();

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello LVGL + SDL2!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    while (1) {
        lv_timer_handler();
        SDL_Delay(5);
    }
    return 0;
}