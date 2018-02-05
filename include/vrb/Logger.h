/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
