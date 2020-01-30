#include "GLCamera.h"

GLCamera::GLCamera() {
    m_radius = 10.0f;
    m_phi = 0.0f;
    m_theta = 0.0f;

    lastX = 400;
    lastY = 300;
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

    m_phi = yaw * (M_PI / 180);
    m_theta = pitch * (M_PI / 180);
}

void GLCamera::change(float x, float y) {
    float sensitivity = 0.005f;
    m_theta2 += x * sensitivity;
    m_phi2 += y * sensitivity;

    m_focus.x = cos(m_theta2) * sin(m_phi2);
    m_focus.y = cos(m_phi2);
    m_focus.z = sin(m_theta2) * sin(m_phi2);
}

void GLCamera::zoom(float distance) {
    m_radius -= distance;
}

Mat4 GLCamera::viewMatrix() {
    /*
        if (firstMouse) {
            lastCamX = width / 2;
            lastCamY = height / 2;
            firstMouse = false;
        }

        float xoffset = (cur_x - lastCamX) / 100.f;
        float yoffset = (lastCamY - cur_y) / 100.f;
        lastCamX = cur_x;
        lastCamY = cur_y;

        Mat4 m1, m2, m3, viewMatrix;

        m1.rotateX(yoffset);
        m2.rotateY(xoffset);
        m3.translate(camPos.x, camPos.y, camPos.z);

        Mat4 mat = (m2 * m1 * m3);
        Vector3f tmp = { mat[12], mat[13], mat[14] };

        camPos = mat * tmp;
        viewMatrix.lookAt(camPos, m_focus, {0, 1, 0});

        return viewMatrix;
    */

    Mat4 viewMatrix;
    Vector3f direction;
    direction.x = m_radius * cos(m_theta) * cos(m_phi); 
    direction.y = m_radius * sin(m_theta); 
    direction.z = m_radius * cos(m_theta) * sin(m_phi); 
    
    viewMatrix.lookAt(direction, m_focus, {0, 1, 0});

    return viewMatrix;
}

void GLCamera::focus(Vector3f v) {
    m_focus = v;
}