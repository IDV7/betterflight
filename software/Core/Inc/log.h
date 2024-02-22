//
// Created by Isaak on 2/20/2024.
//

#ifndef BETTERFLIGHT_LOG_H
#define BETTERFLIGHT_LOG_H

#define ANSI_COLOR(code) ((code) == 0 ? "" : "\033[" #code "m")
#include "misc.h"
#include <stdint.h>

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} log_level_t;

typedef struct {
    log_level_t level;
    bool color;
} log_t;

typedef enum {
    COLOR_RESET = 0,
    COLOR_RED = 31,
    COLOR_GREEN = 32,
    COLOR_YELLOW = 33,
    COLOR_BLUE = 34,
    COLOR_MAGENTA = 35,
    COLOR_CYAN = 36,
    COLOR_WHITE = 37,
    COLOR_DEBUG = COLOR_CYAN,
    COLOR_INFO = COLOR_GREEN,
    COLOR_WARN = COLOR_YELLOW,
    COLOR_ERROR = COLOR_RED,
    COLOR_HIGHLIGHT = COLOR_MAGENTA
} type_color_t;

void log_init(log_level_t log_level, bool color);
void LOGD(const uint8_t *format, ...);
void LOGI(const uint8_t *format, ...);
void LOGW(const uint8_t *format, ...);
void LOGE(const uint8_t *format, ...);
void LOGH(const uint8_t *format, ...);
void LOG(const uint8_t *format, ...);

#endif //BETTERFLIGHT_LOG_H
