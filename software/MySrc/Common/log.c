
#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "misc.h"
#include "mymain.h"

log_handle_t logging;

void log_init(log_level_t log_level, bool color) {
    logging.level = log_level;
    logging.color = color;
}

log_level_t get_log_level() {
    return logging.level;
}

void log_msg(log_level_t log_level, const char *format, va_list args) {
    if (log_level < logging.level) {
        return;
    }

    switch (log_level) {
        case LOG_LEVEL_DEBUG:
            printf("[DEBUG] ");
            break;
        case LOG_LEVEL_INFO:
            printf("[INFO] ");
            break;
        case LOG_LEVEL_WARN:
            printf("[WARN] ");
            break;
        case LOG_LEVEL_ERROR:
            printf("[ERR] ");
            break;
        default:
            break;
    }
    vprintf((char*)format, args);
    printf("\n");
}


void LOGD(const char *format, ...) {
    if (cli_h.disable_log_opt) return; // to save cpu time when needed
    va_list args;
    va_start(args, format);
    log_msg(LOG_LEVEL_DEBUG, format, args);
    va_end(args);
}

void LOGI(const char *format, ...) {
    if (cli_h.disable_log_opt) return; // to save cpu time when needed
    va_list args;
    va_start(args, format);
    log_msg(LOG_LEVEL_INFO, format, args);
    va_end(args);
}

void LOGW(const char *format, ...) {
    if (cli_h.disable_log_opt) return; // to save cpu time when needed
    va_list args;
    va_start(args, format);
    log_msg(LOG_LEVEL_WARN, format, args);
    va_end(args);
}

void LOGE(const char *format, ...) {
    if (cli_h.disable_log_opt) return; // to save cpu time when needed
    va_list args;
    va_start(args, format);
    log_msg(LOG_LEVEL_ERROR, format, args);
    va_end(args);
}

void LOG(const char *format, ...) {
    if (cli_h.disable_log_opt) return; // to save cpu time when needed
    va_list args;
    va_start(args, format);

    vprintf((char*)format, args);
    printf("\r\n");

    va_end(args);
}

// for debugging ascii buffers / strings
void LOG_ascii_hex_dump(char *data) {
    printf("[DEBUG] ASCII HEX DUMP: ");
    for (int i = 0; i < strlen(data); i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}