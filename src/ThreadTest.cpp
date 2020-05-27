#include "vrb/ThreadTest.h"
#include "vrb/ConcreteClass.h"

#include <pthread.h>

namespace vrb {

struct ThreadTest::State {
  pthread_t threadSelf;

  State() : threadSelf(pthread_self()) {}
};

ThreadTestPtr
ThreadTest::Create() {
  return std::make_shared<ConcreteClass<ThreadTest, ThreadTest::State> >();
}

bool
ThreadTest::IsOnInitializationThread() {
  return pthread_equal(m.threadSelf, pthread_self()) > 0;
}

ThreadTest::ThreadTest(State& aState) : m(aState) {}

} // vrb
