//
// Created by Isaak on 2/21/2024.
//

#ifndef BETTERFLIGHT_VERSION_H
#define BETTERFLIGHT_VERSION_H

typedef struct {
    int major;
    int minor;
    int patch;
} version_t;

#define version_to_string(version, version_string) sprintf((char*)version_string, "%d.%d.%d", version.major, version.minor, version.patch)

#define CLI_VERSION (version_t){0, 1, 0}
#define FW_VERSION (version_t){0, 1, 0}

#endif //BETTERFLIGHT_VERSION_H
