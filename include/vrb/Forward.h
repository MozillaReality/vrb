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

class CameraSimple;
typedef std::shared_ptr<CameraSimple> CameraSimplePtr;

class Color;

class Context;
typedef std::shared_ptr<Context> ContextPtr;
typedef std::weak_ptr<Context> ContextWeak;

class CullVisitor;
typedef std::shared_ptr<CullVisitor> CullVisitorPtr;

class Drawable;
typedef std::shared_ptr<Drawable> DrawablePtr;

class DrawableList;
typedef std::shared_ptr<DrawableList> DrawableListPtr;

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

class Group;
typedef std::shared_ptr<Group> GroupPtr;

class Light;
typedef std::shared_ptr<Light> LightPtr;

class Matrix;

class Node;
typedef std::shared_ptr<Node> NodePtr;

class NodeFactoryObj;
typedef std::shared_ptr<NodeFactoryObj> NodeFactoryObjPtr;

class ParserObj;
typedef std::shared_ptr<ParserObj> ParserObjPtr;

class ParserObserverObj;
typedef std::shared_ptr<ParserObserverObj> ParserObserverObjPtr;

class Program;
typedef std::shared_ptr<Program> ProgramPtr;

class ProgramFactory;
typedef std::shared_ptr<ProgramFactory> ProgramFactoryPtr;

class RenderState;
typedef std::shared_ptr<RenderState> RenderStatePtr;

class ResourceGL;

#if defined(ANDROID)
class SurfaceTextureFactory;
typedef std::shared_ptr<SurfaceTextureFactory> SurfaceTextureFactoryPtr;

class SurfaceTextureObserver;
typedef std::shared_ptr<SurfaceTextureObserver> SurfaceTextureObserverPtr;
#endif // defined(ANDROID)

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;

class TextureCache;
typedef std::shared_ptr<TextureCache> TextureCachePtr;

class TextureGL;
typedef std::shared_ptr<TextureGL> TextureGLPtr;

#if defined(ANDROID)
class TextureSurface;
typedef std::shared_ptr<TextureSurface> TextureSurfacePtr;
#endif // defined(ANDROID)

class Transform;
typedef std::shared_ptr<Transform> TransformPtr;

class Updatable;

class Vector;

class VertexArray;
typedef std::shared_ptr<VertexArray> VertexArrayPtr;

} // namespace vrb

#endif // VRB_FORWARD_DOT_H
