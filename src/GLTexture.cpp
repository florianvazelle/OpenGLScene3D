#include "GLTexture.h"
#include "Utils.h"

#include <iostream>

void GLTexture::LoadTexture(const char *path) {
  // 1. chargement de la bitmap
  int w, h, c;
  uint8_t *data = Load(path, w, h, c);
  if (data) {
    // 2. creation du texture object OpenGL
    m_id = 0;
    glGenTextures(1, &m_id);
    // 3. chargement et parametrage
    // pour pouvoir travailler sur/avec la texture
    // on doit d'abord la "bind" / attacher sur un identifiant
    // en l'occurence GL_TEXTURE_2D
    glBindTexture(GL_TEXTURE_2D, m_id);
    // les 6 premiers params definissent le stockage de la texture en VRAM
    // (memoire video) les 3 derniers specifient l'image source
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // on remet la valeur par defaut (pas obligatoire mais preferable ici)
    glBindTexture(GL_TEXTURE_2D, 0);
    // 4. liberation de la memoire
    Free(data);
  } else {
    std::cout << "Error load texture" << std::endl;
  }
}

void GLTexture::DestroyTexture() {
  glDeleteTextures(1, &m_id);
  m_id = 0;
}
