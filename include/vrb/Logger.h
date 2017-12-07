#ifndef VRBROWSER_LOGGER_H
#define VRBROWSER_LOGGER_H

#if defined(ANDROID)
#include <android/log.h>
#define VRLOG(format, ...) __android_log_print(ANDROID_LOG_INFO, "VRBrowser", format, ##__VA_ARGS__);
#define VRLINE VRLOG("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__)
#else
#include <stdio.h>
#define VRLOG(format, ...) fprintf(stderr, "VRBrowser: " format "\n", ##__VA_ARGS__);
#define VRLINE VRLOG("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__)
#endif

#endif //VRBROWSER_LOGGER_H
