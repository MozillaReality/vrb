/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_FORWARD_DOT_H
#define VRB_FORWARD_DOT_H

#include <memory>

namespace vrb {

class Camera;
typedef std::shared_ptr<Camera> CameraPtr;

class CameraEye;
typedef std::shared_ptr<CameraEye> CameraEyePtr;

class CameraSimple;
typedef std::shared_ptr<CameraSimple> CameraSimplePtr;

#if defined(ANDROID)
class ClassLoaderAndroid;
typedef std::shared_ptr<ClassLoaderAndroid> ClassLoaderAndroidPtr;
#endif // defined(ANDROID)

class Color;

class ContextSynchronizer;
typedef std::shared_ptr<ContextSynchronizer> ContextSynchronizerPtr;

class ContextSynchronizerObserver;
typedef std::shared_ptr<ContextSynchronizerObserver> ContextSynchronizerObserverPtr;


class CreationContext;
typedef std::shared_ptr<CreationContext> CreationContextPtr;
typedef std::weak_ptr<CreationContext> CreationContextWeak;

class CullVisitor;
typedef std::shared_ptr<CullVisitor> CullVisitorPtr;

class Drawable;
typedef std::shared_ptr<Drawable> DrawablePtr;

class DrawableList;
typedef std::shared_ptr<DrawableList> DrawableListPtr;

class FBO;
typedef std::shared_ptr<FBO> FBOPtr;

class FileHandler;
typedef std::shared_ptr<FileHandler> FileHandlerPtr;

class FileReader;
typedef std::shared_ptr<FileReader> FileReaderPtr;

#if defined(ANDROID)
class FileReaderAndroid;
typedef std::shared_ptr<FileReaderAndroid> FileReaderAndroidPtr;
#endif // defined(ANDROID)

class Geometry;
typedef std::shared_ptr<Geometry> GeometryPtr;

class GLExtensions;
typedef std::shared_ptr<GLExtensions> GLExtensionsPtr;

class Group;
typedef std::weak_ptr<Group> GroupWeak;
typedef std::shared_ptr<Group> GroupPtr;

class Light;
typedef std::shared_ptr<Light> LightPtr;

class Matrix;

#if defined(ANDROID)
class ModelLoaderAndroid;
typedef std::shared_ptr<ModelLoaderAndroid> ModelLoaderAndroidPtr;
#endif // defined(ANDROID)

class Node;
typedef std::shared_ptr<Node> NodePtr;

class NodeFactoryObj;
typedef std::shared_ptr<NodeFactoryObj> NodeFactoryObjPtr;

class UpdatableStore;

class ParserObj;
typedef std::shared_ptr<ParserObj> ParserObjPtr;

class ParserObserverObj;
typedef std::shared_ptr<ParserObserverObj> ParserObserverObjPtr;

class Program;
typedef std::shared_ptr<Program> ProgramPtr;

class ProgramFactory;
typedef std::shared_ptr<ProgramFactory> ProgramFactoryPtr;

class RenderContext;
typedef std::shared_ptr<RenderContext> RenderContextPtr;
typedef std::weak_ptr<RenderContext> RenderContextWeak;

class RenderState;
typedef std::shared_ptr<RenderState> RenderStatePtr;

class ResourceGL;
class ResourceGLHead;
class ResourceGLTail;

#if defined(ANDROID)
class RunnableQueue;
typedef std::shared_ptr<RunnableQueue> RunnableQueuePtr;

class SurfaceTextureFactory;
typedef std::shared_ptr<SurfaceTextureFactory> SurfaceTextureFactoryPtr;

class SurfaceTextureObserver;
typedef std::shared_ptr<SurfaceTextureObserver> SurfaceTextureObserverPtr;
#endif // defined(ANDROID)

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;

class TextureCache;
typedef std::shared_ptr<TextureCache> TextureCachePtr;

class TextureCubeMap;
typedef std::shared_ptr<TextureCubeMap> TextureCubeMapPtr;

class TextureGL;
typedef std::shared_ptr<TextureGL> TextureGLPtr;

#if defined(ANDROID)
class TextureSurface;
typedef std::shared_ptr<TextureSurface> TextureSurfacePtr;
#endif // defined(ANDROID)

class Toggle;
typedef std::shared_ptr<Toggle> TogglePtr;

class Transform;
typedef std::shared_ptr<Transform> TransformPtr;

class Updatable;
class UpdatableHead;
class UpdatableTail;

class Vector;

class VertexArray;
typedef std::shared_ptr<VertexArray> VertexArrayPtr;

} // namespace vrb

#endif // VRB_FORWARD_DOT_H
