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
GLObject g_Suzanne;
GLCamera g_Camera;

Skybox sb;
FrameBufferObject g_FBO;

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

  g_Suzanne.LoadObject("./assets/models/suzanne.obj", g_BasicShader);

  sb.Init();

  g_FBO.Init(window);
  g_FBO.InitColorBuffer();
}

void Shutdown() {
  g_Suzanne.DestroyObject();
  g_BasicShader.Destroy();
  sb.Destroy();
  g_FBO.Destroy();
}

void Display(GLFWwindow *window) {
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  auto basic = g_BasicShader.GetProgram();

  Mat4 viewMatrix = g_Camera.viewMatrix();

  Mat4 proj3DMatrix;
  float angle = 4000.0f;
  float near = 0.1f;
  float far = 100.0f;
  float aspect = width / height;

  proj3DMatrix.makePerspective((angle * M_PI) / 180, aspect, near, far);

  {
    // Suzanne
    glUseProgram(basic);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glViewport(0, 0, width, height);
    glClearColor(1.f, 0.f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int locMatrix = glGetUniformLocation(basic, "u_proj3DMatrix");
    glUniformMatrix4fv(locMatrix, 1, GL_FALSE, &(proj3DMatrix[0]));

    Mat4 scaleMatrix, translateMatrix;
    scaleMatrix.scale(0.15f, 0.15f, 0.15f);
    glUniformMatrix4fv(glGetUniformLocation(basic, "u_modelMatrix"), 1,
                       GL_FALSE, &(scaleMatrix[0]));
    glUniformMatrix4fv(glGetUniformLocation(basic, "u_viewMatrix"), 1, GL_FALSE,
                       &(viewMatrix[0]));

    Vector3f eye = g_Camera.eyePos();
    glUniform3f(glGetUniformLocation(basic, "cameraPos"), eye.x, eye.y, eye.z);

    glBindTexture(GL_TEXTURE_CUBE_MAP, sb.getSpaceTexId());

    static GLfloat lightPos[3] = {-5.0, 5.0, 5.0};
    glUniform3fv(glGetUniformLocation(basic, "lightPos"), 1, lightPos);

    g_Suzanne.DrawObject();
  }

  {
    // Premier FrameBuffer
    glUseProgram(basic);

    glBindFramebuffer(GL_FRAMEBUFFER, g_FBO.getID());
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glViewport(0, 0, width, height);
    glClearColor(1.f, 0.f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Mat4 depthProjectionMatrix;
    depthProjectionMatrix.ortho(-10, 10, -10, 10, -10, 20);

    Mat4 depthViewMatrix;
    depthViewMatrix.lookAt({5.0f, -5.0f, -5.0f}, {0, 0, 0}, {0, 1, 0});

    int locMatrix = glGetUniformLocation(basic, "u_proj3DMatrix");
    glUniformMatrix4fv(locMatrix, 1, GL_FALSE, &(depthProjectionMatrix[0]));

    Mat4 scaleMatrix, translateMatrix;
    scaleMatrix.scale(5, 5, 5);
    glUniformMatrix4fv(glGetUniformLocation(basic, "u_modelMatrix"), 1,
                       GL_FALSE, &(scaleMatrix[0]));
    glUniformMatrix4fv(glGetUniformLocation(basic, "u_viewMatrix"), 1, GL_FALSE,
                       &(depthViewMatrix[0]));

    Vector3f eye = g_Camera.eyePos();
    glUniform3f(glGetUniformLocation(basic, "cameraPos"), eye.x, eye.y, eye.z);

    glBindTexture(GL_TEXTURE_CUBE_MAP, sb.getSpaceTexId());

    static GLfloat lightPos[3] = {5.0, -5.0, -5.0};
    glUniform3fv(glGetUniformLocation(basic, "lightPos"), 1, lightPos);

    g_Suzanne.DrawObject();
  }

  // En dernier la skybox
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  sb.Draw(viewMatrix, proj3DMatrix);

  // rendu 2D
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  g_FBO.DrawDepthBuffer(width, height, {-0.75, 0.75, 0});
  glDepthMask(GL_TRUE);
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
