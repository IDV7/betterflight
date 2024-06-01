/*

  it was once an idea to let the qt application check the version of the CLI module to see if it was compatible with the firmware version.
  but it was never implemented in the end.

 */


#ifndef BETTERFLIGHT_VERSION_H
#define BETTERFLIGHT_VERSION_H

typedef struct {
    int major;
    int minor;
    int patch;
} version_t;

// macro to convert a version_t to a string
#define VERSION_TO_STRING(version, version_string) sprintf((char*)version_string, "%d.%d.%d", version.major, version.minor, version.patch)


#define CLI_VERSION (version_t){0, 1, 3}
#define FW_VERSION (version_t){0, 1, 0}

#endif //BETTERFLIGHT_VERSION_H
