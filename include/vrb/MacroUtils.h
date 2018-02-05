/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_MACRO_UTILS_DOT_H
#define VRB_MACRO_UTILS_DOT_H

#define VRB_NO_COPY(Class)                 \
  Class(const Class&) = delete;            \
  Class& operator=(const Class&) = delete;

#define VRB_NO_MOVE(Class)           \
  Class(Class&&) = delete;           \
  Class& operator=(Class&&) = delete;

#define VRB_NO_DEFAULTS(Class) \
  VRB_NO_COPY(Class)           \
  VRB_NO_MOVE(Class)

#endif //  VRB_MACRO_UTILS_DOT_H
