#include "vrb/ObjectCounter.h"

#include "vrb/Logger.h"
#include "vrb/Mutex.h"
#include <string>
#include <unordered_map>

namespace vrb {

struct ObjectCounterInfo {
  int32_t count = 0;
  std::string name;
  ObjectCounterInfo(const int32_t aCount, const std::string& aName) :
    count(aCount),
    name(aName) {}
  ObjectCounterInfo(const ObjectCounterInfo& aInfo) = default;
  ObjectCounterInfo& operator=(const ObjectCounterInfo& aInfo) = default;
};

static Mutex* sLock;
static std::unordered_map<std::size_t, ObjectCounterInfo> sMap;

void
InitializeObjectCounter() {
  if (!sLock) {
    sLock = new Mutex();
  }
}

void
LogObjectCount() {
  if (!sLock) {
    return;
  }
  MutexAutoLock lock(*sLock);
  for (const auto& info: sMap) {
    VRB_LOG("Type: %s count: %d", info.second.name.c_str(), info.second.count);
  }
}

void
ShutdownObjectCounter() {
  if (!sLock) {
    return;
  }

  if (sMap.size() == 0) {
    return;
  }

  delete sLock; sLock = nullptr;

  int leakCount = 0;
  for (const auto& info: sMap) {
    if (info.second.count > 0) {
       leakCount++;
       VRB_ERROR("Leak detected: %s count: %d", info.second.name.c_str(), info.second.count);
    }
  }
  if (leakCount == 0) {
    VRB_DEBUG("No leaks detected");
  }
  sMap.clear();
}

void
AddObject(std::size_t aHandle, const char* aName) {
  if (!sLock) {
    return;
  }
  MutexAutoLock lock(*sLock);
  auto search = sMap.find(aHandle);
  if (search != sMap.end()) {
    search->second.count++;
  } else {
    sMap.emplace(aHandle, ObjectCounterInfo(1, aName));
  }
}

void
RemoveObject(std::size_t aHandle) {
  if (!sLock) {
    return;
  }
  MutexAutoLock lock(*sLock);
  auto search = sMap.find(aHandle);
  if (search != sMap.end()) {
    search->second.count--;
  }
}

}
