/*

  The log module is a layer above printf that allows for easy logging of messages with different levels of importance.

 */
#ifndef BETTERFLIGHT_LOG_H
#define BETTERFLIGHT_LOG_H

#include <stdint.h>

#include "misc.h"

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} log_level_t;

typedef struct {
    log_level_t level;
    bool color;
} log_handle_t;

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
log_level_t get_log_level();

// Log debug message
void LOGD(const char *format, ...);

// Log info message
void LOGI(const char *format, ...);

// Log warning message
void LOGW(const char *format, ...);

// Log error message
void LOGE(const char *format, ...);

// Log generic message (no prefix)
void LOG(const char *format, ...);

// Log asiic hex dump
void LOG_ascii_hex_dump(char *data);

#endif //BETTERFLIGHT_LOG_H
