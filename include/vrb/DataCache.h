/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_DATA_CACHE_DOT_H
#define VRB_DATA_CACHE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class DataCache {
public:
  static DataCachePtr Create();
  void SetCachePath(const std::string& aPath);
  uint32_t CacheData(std::unique_ptr<uint8_t[]>& aData, const size_t aDataSize);
  size_t LoadData(const uint32_t aHandle, std::unique_ptr<uint8_t[]>& aData);
  void RemoveData(const uint32_t aHandle);
protected:
  struct State;
  DataCache(State& aState);
  ~DataCache();
private:
  State& m;
  DataCache() = delete;
  VRB_NO_DEFAULTS(DataCache)
};

} // namespace vrb

#endif // VRB_DATA_CACHE_DOT_H
