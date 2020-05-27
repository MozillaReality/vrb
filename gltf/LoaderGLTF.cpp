#include "LoaderGLTF.h"
#include "vrb/ConcreteClass.h"

namespace vrb {

struct LoaderGLTF::State {
};

LoaderGLTFPtr
LoaderGLTF::Create(CreationContextPtr& aContext) {
  LoaderGLTFPtr loader = std::make_shared<ConcreteClass<LoaderGLTF, LoaderGLTF::State> >(aContext);
  return loader;
}

LoaderGLTF::LoaderGLTF(State& aState, CreationContextPtr& aContext) : m(aState) {}

LoaderGLTF::~LoaderGLTF() {}

};

