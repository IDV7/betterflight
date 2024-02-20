//
// Created by Isaak on 2/20/2024.
//
#include "log.h"
#include <stdint.h>
#include <stdio.h>
#include "misc.h"

log_t logging;

void log_init(log_level_t log_level, bool color) {
    logging.level = log_level;
    logging.color = color;
}

uint8_t * get_log_level_color_attr(log_level_t log_level) {
    if (logging.color) {
        switch (log_level) {
            case LOG_LEVEL_DEBUG:
                (uint8_t*)ANSI_COLOR(COLOR_DEBUG);
            case LOG_LEVEL_INFO:
                (uint8_t*)ANSI_COLOR(COLOR_INFO);
            case LOG_LEVEL_WARN:
                (uint8_t*)ANSI_COLOR(COLOR_WARN);
            case LOG_LEVEL_ERROR:
                (uint8_t*)ANSI_COLOR(COLOR_ERROR);
            default:
                return (uint8_t*)"";
        }
    } else {
        return (uint8_t*)"";
    }
}

void log_msg(log_level_t log_level,uint8_t *message) {
    if (log_level < logging.level) {
        return;
    }
    switch (log_level) {
        case LOG_LEVEL_DEBUG:
            printf("%s[DEBUG] %s%s\r\n",get_log_level_color_attr(log_level), message, ANSI_COLOR(COLOR_RESET));
            break;
        case LOG_LEVEL_INFO:
            printf("%s[INFO] %s%s\r\n",get_log_level_color_attr(log_level), message, ANSI_COLOR(COLOR_RESET));
            break;
        case LOG_LEVEL_WARN:
            printf("%s[WARN] %s%s\r\n",get_log_level_color_attr(log_level),message, ANSI_COLOR(COLOR_RESET));
            break;
        case LOG_LEVEL_ERROR:
            printf("%s[ERR] %s%s\r\n",get_log_level_color_attr(log_level),message, ANSI_COLOR(COLOR_RESET));
            break;
        default:
            break;
    }
}

void LOGD(uint8_t *message) {
    log_msg(LOG_LEVEL_DEBUG, message);
}
void LOGI(uint8_t *message) {
    log_msg(LOG_LEVEL_INFO, message);
}
void LOGW(uint8_t *message) {
    log_msg(LOG_LEVEL_WARN, message);
}
void LOGE(uint8_t *message) {
    log_msg(LOG_LEVEL_ERROR, message);
}
void LOGH(uint8_t *message) {
    printf("%s%s%s\n", ANSI_COLOR(COLOR_HIGHLIGHT), message, ANSI_COLOR(COLOR_RESET));
}
void LOG(uint8_t *message) {
    printf("%s%s\n", ANSI_COLOR(COLOR_RESET), message);
}