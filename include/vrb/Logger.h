#ifndef VRBROWSER_LOGGER_H
#define VRBROWSER_LOGGER_H

#if defined(ANDROID)
#include <android/log.h>
#define VRB_LOG(format, ...) __android_log_print(ANDROID_LOG_INFO, "VRB", format, ##__VA_ARGS__);
#define VRB_LINE VRB_LOG("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__)
#else
#include <stdio.h>
#define VRB_LOG(format, ...) fprintf(stderr, "VRB: " format "\n", ##__VA_ARGS__);
#define VRB_LINE VRB_LOG("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__)
#endif

#endif //VRBROWSER_LOGGER_H
