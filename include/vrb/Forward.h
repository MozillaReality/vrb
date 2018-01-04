#ifndef VRB_FORWARD_DOT_H
#define VRB_FORWARD_DOT_H

#include <memory>

namespace vrb {

class Context;
typedef std::shared_ptr<Context> ContextPtr;

class FileHandler;
typedef std::shared_ptr<FileHandler> FileHandlerPtr;

class FileReader;
typedef std::shared_ptr<FileReader> FileReaderPtr;

#if defined(ANDROID)
class FileReaderAndroid;
typedef std::shared_ptr<FileReaderAndroid> FileReaderAndroidPtr;
#endif // defined(ANDROID)

class Matrix;

class ParserObj;
typedef std::shared_ptr<ParserObj> ParserObjPtr;

class ParserObserverObj;
typedef std::shared_ptr<ParserObserverObj> ParserObserverObjPtr;

class RenderObject;
typedef std::shared_ptr<RenderObject> RenderObjectPtr;

class RenderObjectFactoryObj;
typedef std::shared_ptr<RenderObjectFactoryObj> RenderObjectFactoryObjPtr;

class RenderObjectState;
typedef std::shared_ptr<RenderObjectState> RenderObjectStatePtr;

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;

class TextureCache;
typedef std::shared_ptr<TextureCache> TextureCachePtr;

class Vector;

} // namespace vrb

#endif // VRB_FORWARD_DOT_H
