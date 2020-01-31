#include "GLCamera.h"
#include <iostream>

GLCamera::GLCamera() {
    m_radius = 10.0f;
    m_phi = 0.0f;
    m_theta = 0.0f;

    lastX = 1079 / 2;
    lastY = 940 / 2;
    yaw = -90.0f;

    m_focus = { 0, 0, 0 };
}

void GLCamera::rotate(float x, float y) {
    float xoffset = x - lastX;
    float yoffset = lastY - y; 
    lastX = x;
    lastY = y;

    float sensitivity = 0.5f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;

    m_phi = yaw * (M_PI / 180);
    m_theta = pitch * (M_PI / 180);
}

void GLCamera::zoom(float distance) { m_radius -= distance; }

  Mat4 GLCamera::viewMatrix() {
    Vector3f camPos = {0, 0, 3};
    camPos.x = m_radius * cos(m_theta) * cos(m_phi);
    camPos.y = m_radius * sin(m_theta);
    camPos.z = m_radius * cos(m_theta) * sin(m_phi);

    Vector3f camTarget = m_focus;
    Vector3f up = {0, 1, 0};

    Mat4 viewMatrix;
    viewMatrix.lookAt(camPos, camTarget, up);

    return viewMatrix;
}

void GLCamera::focus(Vector3f v) {
    m_focus = v;
}