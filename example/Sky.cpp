#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <iostream>
#include <math.h>
#include <vector>

#include "FBO.h"
#include "GLCamera.h"
#include "GLObject.h"
#include "GLShader.h"
#include "GLTexture.h"
#include "Mat4.h"
#include "Skybox.h"

GLShader g_BasicShader;
GLObject g_Suzanne;
GLCamera g_Camera;

Skybox sb;

void Initialize(GLFWwindow *window) {
  GLenum error = glewInit();

  if (error != GLEW_OK) {
    std::cout << "erreur d'initialisation de GLEW!" << std::endl;
  }

  std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Vendor : " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;

  g_BasicShader.LoadVertexShader("./assets/shaders/Basic.vs");
  g_BasicShader.LoadFragmentShader("./assets/shaders/EnvMapping.fs");
  g_BasicShader.Create();

  g_Suzanne.LoadObject("./assets/models/suzanne.obj", g_BasicShader);

  sb.Init();
}

void Shutdown() {
  g_Suzanne.DestroyObject();
  g_BasicShader.Destroy();
  sb.Destroy();
}

void Display(GLFWwindow *window) {
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  Mat4 viewMatrix = g_Camera.viewMatrix();

  glViewport(0, 0, width, height);
  glClearColor(0, 0, 0, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto basic = g_BasicShader.GetProgram();
  glUseProgram(basic);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  Mat4 proj3DMatrix;
  float angle = 4000.0f;
  float near = 0.1f;
  float far = 100.0f;
  float aspect = width / height;

  proj3DMatrix.makePerspective((angle * M_PI) / 180, aspect, near, far);

  int locMatrix = glGetUniformLocation(basic, "u_proj3DMatrix");
  glUniformMatrix4fv(locMatrix, 1, GL_FALSE, &(proj3DMatrix[0]));

  Mat4 scaleMatrix, translateMatrix;
  scaleMatrix.scale(0.15f, 0.15f, 0.15f);
  glUniformMatrix4fv(glGetUniformLocation(basic, "u_modelMatrix"), 1, GL_FALSE,
                     &(scaleMatrix[0]));
  glUniformMatrix4fv(glGetUniformLocation(basic, "u_viewMatrix"), 1, GL_FALSE,
                     &(viewMatrix[0]));

  Vector3f eye = g_Camera.eyePos();
  glUniform3f(glGetUniformLocation(basic, "cameraPos"), eye.x, eye.y, eye.z);

  glBindTexture(GL_TEXTURE_CUBE_MAP, sb.getSpaceTexId());

  static GLfloat lightPos[3] = {-5.0, 5.0, 5.0};
  glUniform3fv(glGetUniformLocation(basic, "lightPos"), 1, lightPos);
  glUniform1i(glGetUniformLocation(basic, "u_type"), 0);

  glUniform1i(glGetUniformLocation(basic, "u_type"), 2);
  g_Suzanne.DrawObject();

  sb.Draw(viewMatrix, proj3DMatrix);
}

int main(void) {
  GLFWwindow *window;

  if (!glfwInit())
    return -1;

  window = glfwCreateWindow(1079, 940, "OpenGL Scene 3D", NULL, NULL);

  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  Initialize(window);

  while (!glfwWindowShouldClose(window)) {
    Display(window);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  Shutdown();
  glfwTerminate();
  return 0;
}