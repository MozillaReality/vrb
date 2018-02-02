#ifndef VRB_SURFACE_TEXTURE_FACTORY_DOT_H
#define VRB_SURFACE_TEXTURE_FACTORY_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include "vrb/ResourceGL.h"
#include "vrb/Updatable.h"

#include <GLES2/gl2.h>
#include <jni.h>
#include <string>

namespace vrb {

class SurfaceTextureObserver {
public:
  virtual void SurfaceTextureCreated(const std::string& aName, GLuint aHandle, jobject aSurfaceTexture) = 0;
  virtual void SurfaceTextureHandleUpdated(const std::string aName, GLuint aHandle) = 0;
  virtual void SurfaceTextureDestroyed(const std::string& aName) = 0;
  virtual void SurfaceTextureCreationError(const std::string& aName, const std::string& aReason) = 0;
protected:
  SurfaceTextureObserver() {}
  virtual ~SurfaceTextureObserver() {}
private:
  VRB_NO_DEFAULTS(SurfaceTextureObserver)
};

class SurfaceTextureFactory : protected Updatable, protected ResourceGL {
public:
  static SurfaceTextureFactoryPtr Create(ContextWeak& aContext);
  void InitializeJava(JNIEnv* aEnv);
  void ShutdownJava();

  void CreateSurfaceTexture(const std::string& aName, SurfaceTextureObserverPtr aObserver);
  void DestroySurfaceTexture(const std::string& aName);
  void AddGlobalObserver(SurfaceTextureObserverPtr aObserver);
  void RemoveGlobalObserver(const SurfaceTextureObserver& aObserver);

  jobject LookupSurfaceTexture(const std::string& aName);

protected:
  struct State;
  SurfaceTextureFactory(State& aState, ContextWeak& aContext);
  ~SurfaceTextureFactory();

  // Updatable interface
  void UpdateResource(Context& aContext) override;

  // ResourceGL interface
  void InitializeGL(Context& aContext) override;
  void ShutdownGL(Context& aContext) override;

private:
  State& m;
  SurfaceTextureFactory() = delete;
  VRB_NO_DEFAULTS(SurfaceTextureFactory)
};

} // namespace vrb

#endif //  VRB_SURFACE_TEXTURE_FACTORY_DOT_H
