#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* Basic Settings */
#define LV_COLOR_DEPTH 32
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_INFO

/* Disable drivers that require external libs */
#define LV_USE_SDL 1
#define LV_USE_GPU_SDL 0
#define LV_USE_OS 0

/* Memory */
#define LV_MEM_CUSTOM 0
#define LV_MEM_SIZE (128 * 1024U)

/* Tick */
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE <SDL2/SDL.h>
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (SDL_GetTicks())

/* Asserts */
#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MEM           1
#define LV_USE_ASSERT_MEM_INTEGRITY 1

/* Widgets */
#define LV_USE_BTN      1
#define LV_USE_LABEL    1
#define LV_USE_BAR      1
#define LV_USE_SLIDER   1
#define LV_USE_SWITCH   1

#endif
