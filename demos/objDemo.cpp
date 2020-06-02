#include "vrb/AnimatedTransform.h"
#include "vrb/CameraSimple.h"
#include "vrb/CreationContext.h"
#include "vrb/CullVisitor.h"
#include "vrb/DataCache.h"
#include "vrb/DrawableList.h"
#include "vrb/GLError.h"
#include "vrb/Group.h"
#include "vrb/Light.h"
#include "vrb/Logger.h"
#include "vrb/Matrix.h"
#include "vrb/NodeFactoryObj.h"
#include "vrb/ObjectCounter.h"
#include "vrb/ParserObj.h"
#include "vrb/RenderContext.h"
#include "vrb/Vector.h"

#include "vrb/gl.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define WinWidth 1000
#define WinHeight 1000

static bool
PollEvents(SDL_Window* aWindow, const uint32_t aFlags) {
  bool running = true;
  static bool fullScreen = false;
  SDL_Event Event;
  while (SDL_PollEvent(&Event)) {
    if (Event.type == SDL_KEYDOWN) {
      switch (Event.key.keysym.sym) {
        case SDLK_ESCAPE:
          running = false;
          break;
        case 'f':
          fullScreen = !fullScreen;
          if (fullScreen) {
            SDL_SetWindowFullscreen(aWindow, aFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
          }
          else {
            SDL_SetWindowFullscreen(aWindow, aFlags);
          }
          break;
        default:
          break;
      }
    }
    else if (Event.type == SDL_QUIT) {
      running = false;
    }
  }
  return running;
}

int
main(int argc, char* argv[]) {
  vrb::InitializeObjectCounter();
  if (argc != 2) {
    VRB_ERROR("Usage: %s <file name>", argv[0]);
    return 1;
  }

  {
  vrb::RenderContextPtr render = vrb::RenderContext::Create();
  vrb::CreationContextPtr create = render->GetRenderThreadCreationContext();
  render->GetDataCache()->SetCachePath(P_tmpdir);

  uint32_t sdlWindowFlags = SDL_WINDOW_OPENGL;
  SDL_Window* sdlWindow = SDL_CreateWindow("VRB OBJ Loader Test", 0, 0, WinWidth, WinHeight, sdlWindowFlags);
  if (!sdlWindow) {
    VRB_ERROR("SDL Failed to create window.");
    return 1;
  }

  SDL_GLContext sdlContext = SDL_GL_CreateContext(sdlWindow);

  vrb::AnimatedTransformPtr root = vrb::AnimatedTransform::Create(create);
  root->ClearAnimations();
  root->AddRotationAnimation(vrb::Vector(0.0f, 1.0f, 0.0f), vrb::PI_FLOAT / 2.0f);
  root->SetAnimationState(vrb::AnimationState::Play);

  vrb::LightPtr light = vrb::Light::Create(create);
  root->AddLight(light);

  vrb::NodeFactoryObjPtr factory = vrb::NodeFactoryObj::Create(create);
  vrb::ParserObjPtr parser = vrb::ParserObj::Create(create);
  parser->SetFileReader(create->GetFileReader());
  parser->SetObserver(factory);
  vrb::GroupPtr group = vrb::Group::Create(create);
  factory->SetModelRoot(root);
  parser->LoadModel(argv[1]);

  vrb::CameraSimplePtr camera = vrb::CameraSimple::Create(create);
  vrb::CullVisitorPtr cullVisitor = vrb::CullVisitor::Create(create);
  vrb::DrawableListPtr drawList = vrb::DrawableList::Create(create);

  render->InitializeGL();

  VRB_GL_CHECK(glEnable(GL_DEPTH_TEST));
  VRB_GL_CHECK(glEnable(GL_CULL_FACE));
  VRB_GL_CHECK(glEnable(GL_BLEND));
  VRB_GL_CHECK(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));

  camera->SetClipRange(0.1f, 100.0f);

  bool running = true;
  while (running) {
    running = PollEvents(sdlWindow, sdlWindowFlags);

    int width = 0, height = 0;
    SDL_GetWindowSize(sdlWindow, &width, &height);

    camera->SetViewport(width, height);
    if (width > height) {
      camera->SetFieldOfView(60.0f, -1.0f);
    } else {
      camera->SetFieldOfView(-1.0f, 60.0f);
    }

    VRB_GL_CHECK(glViewport(0, 0, width, height));
    VRB_GL_CHECK(glClearColor(0.f, 0.f, 0.f, 0.f));
    VRB_GL_CHECK(glEnable(GL_BLEND));
    VRB_GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    camera->SetTransform(vrb::Matrix::Translation(vrb::Vector(0.0f, 0.0f, 0.2f)));
    render->Update();
    drawList->Reset();
    root->Cull(*cullVisitor, *drawList);
    drawList->Draw(*camera);
    SDL_GL_SwapWindow(sdlWindow);
  }

  vrb::LogObjectCount();

  render->ShutdownGL();
  render = nullptr;
  }
  vrb::ShutdownObjectCounter();
  return 0;
}
