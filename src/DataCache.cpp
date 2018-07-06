/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/DataCache.h"
#include "vrb/ConcreteClass.h"

#include "vrb/Logger.h"
#include "vrb/Mutex.h"

#include <fcntl.h>
#include <string>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <unordered_map>

namespace {
static const std::string sFilePrefix = "/vrb_data_cache_";
struct CachedData {
  std::string path;
  size_t size;

  CachedData() : size(0) {}
  CachedData(const CachedData& aData) : path(aData.path), size(aData.size) {}
  CachedData& operator=(const CachedData& aData) {
    path = aData.path;
    size = aData.size;
    return *this;
  }
};
class CloseFileOnReturn {
  int file;
public:
  CloseFileOnReturn(const int aFile) : file(aFile) {}
  ~CloseFileOnReturn() { if (file >= 0) { close(file); } }
private:
  CloseFileOnReturn() = delete;
  VRB_NO_DEFAULTS(CloseFileOnReturn)
  VRB_NO_NEW_DELETE
};
typedef std::unordered_map<uint32_t, CachedData>::iterator cacheIterator_t;

}

namespace vrb {

struct DataCache::State {
  Mutex cacheLock;
  std::string cachePath;
  uint32_t handleCount;
  std::unordered_map<uint32_t, CachedData> cache;
  State() : handleCount(0) {}
};

DataCachePtr
DataCache::Create() {
  DataCachePtr result = std::make_shared<ConcreteClass <DataCache, DataCache::State> >();
  return result;
}

uint32_t
DataCache::CacheData(std::unique_ptr<uint8_t[]>& aData, const size_t aDataSize) {
  uint32_t handle = 0;
  std::string root;
  {
    MutexAutoLock lock(m.cacheLock);
    root = m.cachePath;
    if (root.empty()) {
      VRB_LOG("Failed to cache data, root path, not set");
      return 0;
    }
    m.handleCount++;
    handle = m.handleCount;
  }

  CachedData info;
  info.size = aDataSize;
  info.path = root + sFilePrefix + std::to_string(handle);
  int file = open(info.path.c_str(), O_WRONLY | O_CREAT, 0660);
  if (file < 0) {
    VRB_LOG("Failed to open cache file: %s for writing", info.path.c_str());
    return 0;
  }
  CloseFileOnReturn hold(file);
  size_t toWrite = aDataSize;
  size_t place = 0;
  while (toWrite > 0) {
    ssize_t written = write(file, &(aData[place]), toWrite);
    if (written < 0) {
      VRB_LOG("Failed writing to cache file: %s", info.path.c_str());
      return 0;
    }
    toWrite -= (size_t)written;
    place = aDataSize - toWrite;
  }
  // Keep data for deletion
  std::unique_ptr<uint8_t[]> data = std::move(aData);
  {
    MutexAutoLock lock(m.cacheLock);
    m.cache[handle] = info;
  }
  VRB_LOG("Cached data: %u size: %u", handle, aDataSize);
  return handle;
}

size_t
DataCache::LoadData(const uint32_t aHandle, std::unique_ptr<uint8_t[]>& aData) {
  cacheIterator_t found;
  {
    MutexAutoLock lock(m.cacheLock);
    found = m.cache.find(aHandle);
    if (found == m.cache.end()) {
      VRB_LOG("Failed to find cache file from handle: %u", aHandle);
      return 0;
    }
  }
  CachedData info = found->second;
  int file = open(info.path.c_str(), O_RDONLY);
  if (file < 0) {
    VRB_LOG("Failed to open cache file: %s for reading", info.path.c_str());
    return 0;
  }
  CloseFileOnReturn hold(file);
  aData = std::make_unique<uint8_t[]>(info.size);
  size_t toRead = info.size;
  size_t place = 0;
  while (toRead > 0) {
    ssize_t dataRead = read(file, &(aData[place]), toRead);
    if (dataRead < 0) {
      VRB_LOG("Failed to read from cache file: %s", info.path.c_str());
      return 0;
    }
    toRead -= (size_t)dataRead;
    place = info.size - toRead;
  }
  VRB_LOG("Loaded cached data: %u size: %u", aHandle, info.size);
  return info.size;
}

void
DataCache::RemoveData(const uint32_t aHandle) {
  std::string path;
  {
    MutexAutoLock lock(m.cacheLock);
    cacheIterator_t found = m.cache.find(aHandle);
    if (found == m.cache.end()) {
      VRB_LOG("Failed to find cache file for removal from handle: %u", aHandle);
      return;
    }
    path = found->second.path;
    m.cache.erase(found);
  }
  if (remove(path.c_str()) < 0) {
    VRB_LOG("Failed to remove cache file: %s", path.c_str());
  }
}

void
DataCache::SetCachePath(const std::string& aPath) {
  VRB_LOG("Setting cache root path: %s", aPath.c_str());
  MutexAutoLock lock(m.cacheLock);
  m.cachePath = aPath;
}

DataCache::DataCache(State& aState) : m(aState) {}
DataCache::~DataCache() {
  // No need to lock since if destructor is called, no references are left
  for (cacheIterator_t info = m.cache.begin(); info != m.cache.end(); info++) {
    if (remove(info->second.path.c_str()) < 0) {
      VRB_LOG("Failed to remove cache file: %s", info->second.path.c_str());
    }
  }
}

} // namespace vrb
