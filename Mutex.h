#ifndef VRB_MUTEX_DOT_H
#define VRB_MUTEX_DOT_H

#include <pthread.h>

namespace vrb {

class Mutex {
public:
  Mutex() {
    pthread_mutex_init(&mMutex, nullptr);
  }
  ~Mutex() {
    pthread_mutex_destroy(&mMutex);
  }
  bool Lock() {
    return pthread_mutex_lock(&mMutex) == 0;
  }
  bool TryLock() {
    return pthread_mutex_trylock(&mMutex) == 0;
  }
  bool Unlock() {
    return pthread_mutex_unlock(&mMutex) == 0;
  }
protected:
  pthread_mutex_t mMutex;
};

} // namespace vrb

#endif //  VRB_MUTEX_DOT_H
