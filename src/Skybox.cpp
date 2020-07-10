#include "Skybox.h"
#include "Utils.h"
#include <iostream>

GLuint Skybox::load_cube_map_sprite_texture(const char *path_sprite[6]) {
  GLuint _texId;

  int w, h, c;
  uint8_t *xpos = Load(path_sprite[0], w, h, c);
  uint8_t *xneg = Load(path_sprite[1], w, h, c);
  uint8_t *ypos = Load(path_sprite[2], w, h, c);
  uint8_t *yneg = Load(path_sprite[3], w, h, c);
  uint8_t *zpos = Load(path_sprite[4], w, h, c);
  uint8_t *zneg = Load(path_sprite[5], w, h, c);

  // generate a cube-map texture to hold all the sides
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_CUBE_MAP);

  glGenTextures(1, &_texId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _texId);

  // load each image and copy into a side of the cube-map texture
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, w, h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, xpos);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, w, h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, xneg);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, w, h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, ypos);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, w, h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, yneg);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, w, h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, zpos);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, w, h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, zneg);

  // format cube map texture
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  Free(xneg);
  Free(xpos);
  Free(yneg);
  Free(ypos);
  Free(zneg);
  Free(zpos);

  glBindTexture(GL_TEXTURE_2D, 0);

  return _texId;
}

void Skybox::Init() {
  /* Chargement des shaders pour la skybox */
  g_SkyboxShader.LoadVertexShader("./assets/shaders/Skybox.vs");
  g_SkyboxShader.LoadFragmentShader("./assets/shaders/Skybox.fs");
  g_SkyboxShader.Create();

  /* Chargement de la texture cube */
  const char *skybox_path_sprite[6] = {
      "assets/textures/skybox/rigth.png", "assets/textures/skybox/left.png",
      "assets/textures/skybox/top.png",   "assets/textures/skybox/bottom.png",
      "assets/textures/skybox/front.png", "assets/textures/skybox/back.png"};
  _spaceTexId = load_cube_map_sprite_texture(skybox_path_sprite);

  GLfloat skyboxVertices[] = {
      // positions
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

  GLuint skyboxVBO = 0;

  /* Configuration de la skybox VAO */

  glGenBuffers(1, &skyboxVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices,
               GL_STATIC_DRAW);

  glGenVertexArrays(1, &skyboxVAO);
  glBindVertexArray(skyboxVAO);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
}

void Skybox::_check_gl_error(const char *file, int line) {
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

void Skybox::Draw(Mat4 viewMatrix, Mat4 proj3DMatrix) {
  // glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);

  auto shader = g_SkyboxShader.GetProgram();
  glUseProgram(shader);

  viewMatrix.translate(0, 0, 0);
  glUniformMatrix4fv(glGetUniformLocation(shader, "u_viewMatrix"), 1, GL_FALSE,
                     &(viewMatrix[0]));

  int locMatrix = glGetUniformLocation(shader, "u_proj3DMatrix");
  glUniformMatrix4fv(locMatrix, 1, GL_FALSE, &(proj3DMatrix[0]));

  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _spaceTexId);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);

  glUseProgram(0);
  // glDepthMask(GL_TRUE);
}

void Skybox::Destroy() { g_SkyboxShader.Destroy(); }
