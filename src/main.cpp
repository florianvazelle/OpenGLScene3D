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

  sb.Init();
}

void Shutdown() { sb.Destroy(); }

void Display(GLFWwindow *window) {
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  Mat4 viewMatrix = g_Camera.viewMatrix();

  glViewport(0, 0, width, height);
  glClearColor(0, 0, 0, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Mat4 proj3DMatrix;
  float angle = 4000.0f;
  float near = 0.1f;
  float far = 100.0f;
  float aspect = width / height;

  proj3DMatrix.makePerspective((angle * M_PI) / 180, aspect, near, far);
  sb.Draw(viewMatrix, proj3DMatrix);
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
