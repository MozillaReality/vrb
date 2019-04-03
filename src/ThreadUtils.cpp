/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <vrb/Logger.h>
#include <vrb/ThreadUtils.h>

#include <sys/prctl.h>

namespace vrb {
void SetThreadName(const std::string& aName) {
  prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(aName.c_str()));
}

} // namespace vrb

