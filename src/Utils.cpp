#include <GL/glew.h>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

uint8_t *Load(const char *path, int &w, int &h, int &c) {
  return stbi_load(path, &w, &h, &c, STBI_rgb_alpha);
}

void Free(uint8_t *data) { stbi_image_free(data); }

void _check_gl_error(const char *file, int line) {
  GLenum err(glGetError());
  std::string error;

  switch (err) {
  case GL_INVALID_OPERATION:
    error = "INVALID_OPERATION";
    break;
  case GL_INVALID_ENUM:
    error = "INVALID_ENUM";
    break;
  case GL_INVALID_VALUE:
    error = "INVALID_VALUE";
    break;
  case GL_OUT_OF_MEMORY:
    error = "OUT_OF_MEMORY";
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    error = "INVALID_FRAMEBUFFER_OPERATION";
    break;
  }

  std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line
            << std::endl;
  if (err != GL_NO_ERROR) {
    exit(1);
  }
}