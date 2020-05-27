cmake_minimum_required(VERSION 3.4.1)

if (APPLE)
    find_package(OpenGL REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fexceptions -frtti -Werror -Wno-int-to-void-pointer-cast")
endif ()

include_directories("../include" "../third_party")
add_library(
        #library name
        vrb
        STATIC
        AnimatedTransform.cpp
        BasicShaders.cpp
        BlockTimer.cpp
        CameraEye.cpp
        CameraSimple.cpp
        ContextSynchronizer.cpp
        CreationContext.cpp
        CullVisitor.cpp
        DataCache.cpp
        Drawable.cpp
        DrawableList.cpp
        FileReaderBasic.cpp
        FBO.cpp
        GLError.cpp
        GLExtensions.cpp
        Geometry.cpp
        GeometryDrawable.cpp
        Group.cpp
        Light.cpp
        Math.cpp
        Node.cpp
        NodeFactoryObj.cpp
        ObjectCounter.cpp
        ParserObj.cpp
        PerformanceMonitor.cpp
        Program.cpp
        ProgramFactory.cpp
        Quaternion.cpp
        RenderBuffer.cpp
        RenderContext.cpp
        RenderState.cpp
        ResourceGL.cpp
        ShaderUtil.cpp
        Texture.cpp
        TextureCache.cpp
        TextureCubeMap.cpp
        TextureGL.cpp
        ThreadIdentity.cpp
        Toggle.cpp
        Transform.cpp
        Updatable.cpp
        VertexArray.cpp
)

if (ANDROID)
    target_sources(
            vrb
            PRIVATE
            ClassLoaderAndroid.cpp
            EGLError.cpp
            FileReaderAndroid.cpp
            ModelLoaderAndroid.cpp
            RunnableQueue.cpp
            SharedEGLContext.cpp
            SurfaceTextureFactory.cpp
            TextureSurface.cpp
            ThreadUtils.cpp
    )
endif ()
