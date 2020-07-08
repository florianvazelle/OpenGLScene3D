#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

class GLTexture {
public:
  void LoadTexture(const char *path);
  void DestroyTexture(void);
  GLuint GetTexture() const { return m_id; };

private:
  GLuint m_id;
};

#endif