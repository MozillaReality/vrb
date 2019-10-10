#include "vrb/ThreadIdentity.h"
#include "vrb/ConcreteClass.h"

#include <pthread.h>

namespace vrb {

struct ThreadIdentity::State {
  pthread_t self;
  State() : self(pthread_self()) {}
};

ThreadIdentityPtr
ThreadIdentity::Create() {
  return std::make_shared<ConcreteClass<ThreadIdentity, ThreadIdentity::State> >();
}

bool
ThreadIdentity::IsOnInitializationThread() const {
  return pthread_equal(m.self, pthread_self()) > 0;
}

bool
ThreadIdentity::IsSameThread(const ThreadIdentity& aIdentity) const {
  return pthread_equal(m.self, aIdentity.m.self) > 0;
}


ThreadIdentity::ThreadIdentity(State& aState) : m(aState) {}

} // vrb
