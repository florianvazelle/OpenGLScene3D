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

bool arcball_on = false, target_on = false;

GLShader g_BasicShader;
GLObject g_Suzanne[3], g_Spaceship;
GLCamera g_Camera;
GLTexture g_Batman;

FrameBufferObject g_FBO[2];

Skybox sb;

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

void Initialize(GLFWwindow *window) {
  GLenum error = glewInit();

  if (error != GLEW_OK) {
    std::cout << "erreur d'initialisation de GLEW!" << std::endl;
  }

  std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Vendor : " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;

  g_BasicShader.LoadVertexShader("./assets/shaders/Basic.vs");
  g_BasicShader.LoadFragmentShader("./assets/shaders/Basic.fs");
  g_BasicShader.Create();

  g_Batman.LoadTexture("./assets/textures/batman_logo.png");

  g_Suzanne[0].LoadObject("./assets/models/suzanne.obj", g_BasicShader);
  g_Suzanne[1].LoadObject("./assets/models/suzanne.obj", g_BasicShader);
  g_Suzanne[2].LoadObject("./assets/models/suzanne.obj", g_BasicShader);
  g_Spaceship.LoadObject("./assets/models/Crate1.obj", g_BasicShader);

  sb.Init();

  g_FBO[0].Init(window);
  g_FBO[0].InitColorBuffer();

  g_FBO[1].Init(window);
  g_FBO[1].InitColorBuffer();
}

void Shutdown() {
  g_Suzanne[0].DestroyObject();
  g_Suzanne[1].DestroyObject();
  g_Suzanne[2].DestroyObject();
  g_Spaceship.DestroyObject();
  g_BasicShader.Destroy();

  sb.Destroy();
  g_FBO[0].Destroy();
  g_FBO[1].Destroy();
}

void Display(GLFWwindow *window) {
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  auto basic = g_BasicShader.GetProgram();
  glUseProgram(basic);

  Mat4 scaleMatrix, rotateMatrix, translateMatrix;
  Mat4 viewMatrix = g_Camera.viewMatrix();

  // Premier FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, g_FBO[0].getID());
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  glViewport(0, 0, width, height);
  glClearColor(1.f, 0.f, 0.5f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  scaleMatrix.scale(0.05f, 0.05f, 0.05f);
  glUniformMatrix4fv(glGetUniformLocation(basic, "u_modelMatrix"), 1, GL_FALSE,
                     &(scaleMatrix[0]));
  glUniformMatrix4fv(glGetUniformLocation(basic, "u_viewMatrix"), 1, GL_FALSE,
                     &(viewMatrix[0]));

  glUniform1i(glGetUniformLocation(basic, "u_type"), 2);
  g_Suzanne[1].DrawObject();

  // Second FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, g_FBO[1].getID());
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  glViewport(0, 0, width, height);
  glClearColor(1.f, 0.f, 0.5f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  scaleMatrix.scale(0.05f, 0.05f, 0.05f);
  glUniformMatrix4fv(glGetUniformLocation(basic, "u_modelMatrix"), 1, GL_FALSE,
                     &(scaleMatrix[0]));
  glUniformMatrix4fv(glGetUniformLocation(basic, "u_viewMatrix"), 1, GL_FALSE,
                     &(viewMatrix[0]));

  glUniform1i(glGetUniformLocation(basic, "u_type"), 2);
  g_Suzanne[1].DrawObject();

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

  // On dessine dans le FrameBuffer de base
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  glViewport(0, 0, width, height);
  glClearColor(1.f, 1.f, 1.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  scaleMatrix.scale(0.05f, 0.05f, 0.05f);
  glUniformMatrix4fv(glGetUniformLocation(basic, "u_modelMatrix"), 1, GL_FALSE,
                     &(scaleMatrix[0]));

  glUniformMatrix4fv(glGetUniformLocation(basic, "u_viewMatrix"), 1, GL_FALSE,
                     &(viewMatrix[0]));

  Mat4 proj3DMatrix;
  float angle = 90.0f;
  float near = 0.1f;
  float far = 100.0f;
  float aspect = width / height;
  proj3DMatrix.makePerspective((angle * M_PI) / 180, aspect, near, far);
  int locMatrix = glGetUniformLocation(basic, "u_proj3DMatrix");
  glUniformMatrix4fv(locMatrix, 1, GL_FALSE, &(proj3DMatrix[0]));

  static GLfloat lightPos[3] = {-5.0, 5.0, 5.0};
  glUniform3fv(glGetUniformLocation(basic, "lightPos"), 1, lightPos);
  glUniform1i(glGetUniformLocation(basic, "u_type"), 0);

  g_Suzanne[0].DrawObject();

  // Troisième Suzanne en Texture
  translateMatrix.translate(0.15, 0, 0);
  glUniformMatrix4fv(glGetUniformLocation(basic, "u_modelMatrix"), 1, GL_FALSE,
                     &((translateMatrix * scaleMatrix)[0]));

  glBindTexture(GL_TEXTURE0, g_Batman.GetTexture());
  glUniform1i(glGetUniformLocation(basic, "u_type"), 2);

  g_Suzanne[1].DrawObject();

  glBindTexture(GL_TEXTURE0, 0);

  // Spaceship
  translateMatrix.translate(-0.15, 0, 0);
  glUniformMatrix4fv(glGetUniformLocation(basic, "u_modelMatrix"), 1, GL_FALSE,
                     &((translateMatrix * scaleMatrix)[0]));
  glUniform1i(glGetUniformLocation(basic, "u_type"), 1);

  g_Spaceship.DrawObject();

  sb.Draw();

  // rendu 2D
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  g_FBO[0].DrawColorBuffer(width, height, {-0.75, 0.75, 0});
  g_FBO[1].DrawColorBuffer(width, height, {0.75, 0.75, 0}, 2);
}

void onMouse(GLFWwindow *window, int button, int state, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_PRESS) {
    arcball_on = true;
  } else {
    arcball_on = false;
  }

  if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_PRESS) {
    target_on = true;
  } else {
    target_on = false;
  }
}

void onMotion(GLFWwindow *window, double x, double y) {
  if (arcball_on) {
    g_Camera.rotate(x, y);
  }
  if (target_on) {
    g_Camera.change(x, y);
  }
}

void onScroll(GLFWwindow *window, double x, double y) { g_Camera.zoom(y); }

void onKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    g_Camera.focus({0, 0, 0});
  } else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    g_Camera.focus({0.15, 0, 0});
  } else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
    g_Camera.focus({-0.15, 0, 0});
  }
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

  glfwSetCursorPosCallback(window, onMotion);
  glfwSetMouseButtonCallback(window, onMouse);
  glfwSetScrollCallback(window, onScroll);
  glfwSetKeyCallback(window, onKey);

  while (!glfwWindowShouldClose(window)) {
    Display(window);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  Shutdown();
  glfwTerminate();
  return 0;
}
