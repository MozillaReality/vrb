/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRBROWSER_LOGGER_H
#define VRBROWSER_LOGGER_H

#if defined(ANDROID)
#  include <android/log.h>
#  if defined(NDEBUG)
#    define VRB_DEBUG(...)
#  else
#    define VRB_DEBUG(format, ...) __android_log_print(ANDROID_LOG_DEBUG, "VRB", format, ##__VA_ARGS__);
#  endif
#  define VRB_LOG(format, ...) __android_log_print(ANDROID_LOG_INFO, "VRB", format, ##__VA_ARGS__);
#  define VRB_WARN(format, ...) __android_log_print(ANDROID_LOG_WARN, "VRB", format, ##__VA_ARGS__);
#  define VRB_ERROR(format, ...) __android_log_print(ANDROID_LOG_ERROR, "VRB", format, ##__VA_ARGS__);
#else
#  include <stdio.h>
#  define VRB_DEBUG(format, ...) fprintf(stderr, "VRB DEBUG: " format "\n", ##__VA_ARGS__);
#  define VRB_LOG(format, ...) fprintf(stderr, "VRB: " format "\n", ##__VA_ARGS__);
#  define VRB_WARN(format, ...) fprintf(stderr, "VRB WARNING: " format "\n", ##__VA_ARGS__);
#  define VRB_ERROR(format, ...) fprintf(stderr, "VRB ERROR: " format "\n", ##__VA_ARGS__);
#endif
#define VRB_LINE VRB_DEBUG("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__);

#endif //VRBROWSER_LOGGER_H
