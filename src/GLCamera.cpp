#include "GLCamera.h"
#include <GLFW/glfw3.h>
#include <iostream>

GLCamera::GLCamera() {
  m_radius = 50.0f;
  m_phi = 0.0f;
  m_theta = 0.0f;

  lastX = width / 2;
  lastY = height / 2;
  yaw = -90.0f;

  m_focus = {0, 0, 0};
  eye = {0, 0, 3};
  lookat = {0, 0, 0};
}

void GLCamera::rotate(float x, float y) {
  float xoffset = x - lastX;
  float yoffset = lastY - y;
  lastX = x;
  lastY = y;

  float sensitivity = 0.5f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  m_phi = yaw * (M_PI / 180);
  m_theta = pitch * (M_PI / 180);
}

void GLCamera::change(float x, float y) {
  eye.x -= transScale * (x - lastX) / (float)width;
  lookat.x -= transScale * (x - lastX) / (float)width;
  eye.y += transScale * (y - lastY) / (float)height;
  lookat.y += transScale * (y - lastY) / (float)height;

  lastX = x;
  lastY = y;
}

void GLCamera::zoom(float distance) { m_radius -= distance; }

Mat4 GLCamera::viewMatrix() {
  eye.x = m_radius * cos(m_theta) * cos(m_phi);
  eye.y = m_radius * sin(m_theta);
  eye.z = m_radius * cos(m_theta) * sin(m_phi);

  const float radius = 50.0f;
  float camX = sin(glfwGetTime() / 2);
  float camZ = cos(glfwGetTime() / 2);

  Mat4 viewMatrix;
  viewMatrix.lookAt({0.0, 0.0, 0.0}, {camX, 0.0, camZ}, {0, 1, 0});
  // viewMatrix.lookAt(eye, lookat, {0, 1, 0});

  return viewMatrix;
}

void GLCamera::focus(Vector3f v) { lookat = v; }