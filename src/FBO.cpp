#include <cassert>

#include "FBO.h"
#include "Mat4.h"

void FrameBufferObject::Init(GLFWwindow *window) {
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  g_TextureShader.LoadVertexShader("./assets/shaders/Texture.vs");
  g_TextureShader.LoadFragmentShader("./assets/shaders/Texture.fs");
  g_TextureShader.Create();

  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  // Color Buffer
  glGenTextures(1, &m_colorbuffer);
  glBindTexture(GL_TEXTURE_2D, m_colorbuffer);

  // Screen size and data at NULL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_colorbuffer, 0);

  glGenTextures(1, &m_depthbuffer);
  glBindTexture(GL_TEXTURE_2D, m_depthbuffer);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Depth Buffer
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0,
               GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         m_depthbuffer, 0);

  // glDrawBuffer(GL_NONE);
  // glReadBuffer(GL_NONE);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  //_check_gl_error("TroisiemeProjet", 210);
}

void FrameBufferObject::InitColorBuffer() {
  const float triangles[] = {
      -0.5f,
      -0.5f,
      /*uv*/ 0.f,
      0.f,
      -0.5f,
      +0.5f,
      /*uv*/ 0.f,
      1.f,
      +0.5f,
      +0.5f,
      /*uv*/ 1.f,
      1.f,
      // second triangle
      -0.5f,
      -0.5f,
      /*uv*/ 0.f,
      0.f,
      +0.5f,
      +0.5f,
      /*uv*/ 1.f,
      1.f,
      +0.5f,
      -0.5f,
      /*uv*/ 1.f,
      0.f,
  };

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

  auto program = g_TextureShader.GetProgram();

  int texture_location = glGetUniformLocation(program, "u_TextureSampler");
  glUniform1i(texture_location, 0);

  int position_location = glGetAttribLocation(program, "a_position");
  glEnableVertexAttribArray(position_location);
  glVertexAttribPointer(position_location, 2, GL_FLOAT, false,
                        sizeof(float) * 4, 0);

  int texcoords_location = glGetAttribLocation(program, "a_texcoords");
  glEnableVertexAttribArray(texcoords_location);
  glVertexAttribPointer(texcoords_location, 2, GL_FLOAT, false,
                        sizeof(float) * 4, (const void *)(sizeof(float) * 2));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FrameBufferObject::DrawColorBuffer(int width, int height, Vector3f pos,
                                        int type) {
  auto program = g_TextureShader.GetProgram();
  glUseProgram(program);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_colorbuffer);

  GLfloat dimensions[2] = {(float)width, (float)height};
  glUniform2fv(glGetUniformLocation(program, "dimensions"), 1, dimensions);
  glUniform1i(glGetUniformLocation(program, "u_type"), type);

  Mat4 scale, trans;
  scale.scale(0.5, 0.5, 1);
  trans.translate(pos.x, pos.y, pos.z);
  glUniformMatrix4fv(glGetUniformLocation(program, "u_modelMatrix"), 1,
                     GL_FALSE, &((trans * scale)[0]));

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBufferObject::DrawDepthBuffer(int width, int height, Vector3f pos,
                                        int type) {
  auto program = g_TextureShader.GetProgram();
  glUseProgram(program);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_depthbuffer);

  GLfloat dimensions[2] = {(float)width, (float)height};
  glUniform2fv(glGetUniformLocation(program, "dimensions"), 1, dimensions);

  Mat4 scale, trans;
  scale.scale(0.5, 0.5, 1);
  trans.translate(pos.x, pos.y, pos.z);
  glUniformMatrix4fv(glGetUniformLocation(program, "u_modelMatrix"), 1,
                     GL_FALSE, &((trans * scale)[0]));

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBufferObject::Destroy() {
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);

  glDeleteTextures(1, &m_colorbuffer);
  glDeleteTextures(1, &m_depthbuffer);

  glDeleteFramebuffers(1, &FBO);
}