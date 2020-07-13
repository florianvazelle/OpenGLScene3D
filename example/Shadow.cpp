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

GLShader g_BasicShader, g_DepthBasicShader;
GLObject g_Suzanne[2];
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

  g_DepthBasicShader.LoadVertexShader("./assets/shaders/DepthBasic.vs");
  g_DepthBasicShader.LoadFragmentShader("./assets/shaders/DepthBasic.fs");
  g_DepthBasicShader.Create();

  g_Suzanne[0].LoadObject("./assets/models/scene.obj", g_BasicShader);
  g_Suzanne[1].LoadObject("./assets/models/scene.obj", g_DepthBasicShader);

  sb.Init();

  g_FBO.Init(window);
  g_FBO.InitColorBuffer();
}

void Shutdown() {
  g_Suzanne[0].DestroyObject();
  g_Suzanne[1].DestroyObject();
  g_BasicShader.Destroy();
  g_DepthBasicShader.Destroy();
  sb.Destroy();
  g_FBO.Destroy();
}

void Display(GLFWwindow *window) {
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  const Vector3f eye = g_Camera.eyePos();

  const GLfloat cameraPos[3] = {eye.x + 20, eye.y, eye.z + 20};
  const GLfloat lightPos[3] = {-2.0f, 4.0f, -1.0f};

  float angle = 4000.0f;
  float near = 0.1f;
  float far = 100.0f;
  float aspect = width / height;

  Mat4 proj3DMatrix, viewMatrix;
  proj3DMatrix.makePerspective((angle * M_PI) / 180, aspect, near, far);
  viewMatrix = g_Camera.viewMatrix();

  Mat4 depthViewMatrix, depthProjectionMatrix;
  depthProjectionMatrix.ortho(-2, 2, -2, 2, 2.0f, 7.5f);
  depthViewMatrix.lookAt({lightPos[0], lightPos[1], lightPos[2]}, {0, 0, 0},
                         {0, 1, 0});

  Mat4 bias;
  bias.data[0] = 0.5;
  bias.data[5] = 0.5;
  bias.data[10] = 0.5;

  bias.data[12] = 0.5;
  bias.data[13] = 0.5;
  bias.data[14] = 0.5;

  // 1. render depth of scene to texture (from light's
  // perspective)
  {
    // Dessine les objects dans le Depth FrameBuffer,
    // avec un shader simple, pour avoir une image clean
    auto basic = g_DepthBasicShader.GetProgram();
    glUseProgram(basic);

    glBindFramebuffer(GL_FRAMEBUFFER, g_FBO.getID());
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT); // Peter panning
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glViewport(0, 0, width, height);
    glClear(GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(glGetUniformLocation(basic, "u_proj3DMatrix"), 1,
                       GL_FALSE, &(depthProjectionMatrix[0]));

    Mat4 scaleMatrix;
    // scaleMatrix.scale(0.5f, 0.5f, 0.5f);
    glUniformMatrix4fv(glGetUniformLocation(basic, "u_modelMatrix"), 1,
                       GL_FALSE, &(scaleMatrix[0]));
    glUniformMatrix4fv(glGetUniformLocation(basic, "u_viewMatrix"), 1, GL_FALSE,
                       &(depthViewMatrix[0]));

    g_Suzanne[1].DrawObject();
  }

  // 2. then render scene as normal with shadow mapping (using depth map)
  {
    auto basic = g_BasicShader.GetProgram();
    glUseProgram(basic);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glViewport(0, 0, width, height);
    glClearColor(1.f, 0.f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(glGetUniformLocation(basic, "u_proj3DMatrix"), 1,
                       GL_FALSE, &(proj3DMatrix[0]));

    Mat4 scaleMatrix;
    // scaleMatrix.scale(0.5f, 0.5f, 0.5f);
    glUniformMatrix4fv(glGetUniformLocation(basic, "u_modelMatrix"), 1,
                       GL_FALSE, &(scaleMatrix[0]));
    glUniformMatrix4fv(glGetUniformLocation(basic, "u_viewMatrix"), 1, GL_FALSE,
                       &(viewMatrix[0]));

    glUniformMatrix4fv(
        glGetUniformLocation(basic, "u_depthMVP"), 1, GL_FALSE,
        &((bias * (depthProjectionMatrix * depthViewMatrix))[0]));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_FBO.getDepth());

    glUniform3fv(glGetUniformLocation(basic, "cameraPos"), 1, cameraPos);
    glUniform3fv(glGetUniformLocation(basic, "lightPos"), 1, lightPos);

    g_Suzanne[0].DrawObject();
  }

  // En dernier la skybox
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    sb.Draw(viewMatrix, proj3DMatrix);
  }

  // rendu 2D
  {
    // render Depth map to quad for visual debugging
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    g_FBO.DrawDepthBuffer(width, height, {-0.75, 0.75, 0});
  }
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
