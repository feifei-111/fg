#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION

#ifdef _WIN32
#define GLAD_WGL_IMPLEMENTATION
#include <glad/wgl.h>
#undef GLAD_WGL_IMPLEMENTATION
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION
