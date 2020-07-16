#include <iostream>
#include <math.h>

#include "Mat4.h"

Mat4::Mat4() { identity(); }

Mat4::Mat4(const Mat4 &m) {
  for (int i = 0; i < 16; i++) {
    data[i] = m[i];
  }
}

Mat4 Mat4::operator*(const Mat4 &m) {
  Mat4 res;
  float sum;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      sum = 0;
      for (int k = 0; k < 4; k++) {
        sum = sum + m[(i * 4) + k] * data[(k * 4) + j];
      }
      res[(i * 4) + j] = sum;
    }
  }
  return res;
}

float &Mat4::operator[](int i) { return data[i]; }

const float &Mat4::operator[](int i) const { return data[i]; }

void Mat4::identity() {
  for (int i = 0; i < 16; i++) {
    if (i % 5 == 0) {
      data[i] = 1.0f;
    } else {
      data[i] = 0.0f;
    }
  }
}

void Mat4::display() {
  for (int i = 0; i < 16; i++) {
    if (i % 4 == 0) {
      std::cout << std::endl;
    }
    std::cout << data[i] << ' ';
  }
  std::cout << std::endl;
}

void Mat4::scale(float sx, float sy, float sz) {
  data[0] = sx;
  data[5] = sy;
  data[10] = sz;
}

void Mat4::translate(float x, float y, float z) {
  data[12] = x;
  data[13] = y;
  data[14] = z;
}

void Mat4::rotateX(float angle) {
  data[5] = cos(angle);
  data[6] = -sin(angle);
  data[9] = sin(angle);
  data[10] = cos(angle);
}

void Mat4::rotateY(float angle) {
  data[0] = cos(angle);
  data[2] = -sin(angle);
  data[8] = sin(angle);
  data[10] = cos(angle);
}

void Mat4::rotateZ(float angle) {
  data[0] = cos(angle);
  data[1] = sin(angle);
  data[4] = -sin(angle);
  data[5] = cos(angle);
}

void Mat4::ortho(float left, float right, float bottom, float top, float near,
                 float far) {
  data[0] = 2 / (right - left);
  data[12] = -(right + left) / (right - left);
  data[5] = 2 / (top - bottom);
  data[13] = -(top + bottom) / (top - bottom);
  data[10] = -2 / (far - near);
  data[14] = -(far + near) / (far - near);
}

void Mat4::proj3D(float fovy, float aspect, float near, float far) {
  // cot => cotangents
  float f = 1 / tan(fovy / 2);
  float rangeInv = 1.0 / (near - far);

  data[0] = f / aspect;
  data[5] = f;
  data[10] = (near + far) * rangeInv;
  data[11] = -1;
  data[14] = near * far * rangeInv * 2;
  data[15] = 0;
}

void Mat4::makePerspective(float fieldOfViewInRadians, float aspect, float near,
                           float far) {
  float ymax, xmax;
  ymax = near * tanf(fieldOfViewInRadians * M_PI / 360.0);
  xmax = ymax * aspect;
  frustum(-xmax, xmax, -ymax, ymax, near, far);
}

void Mat4::frustum(float left, float right, float bottom, float top, float near,
                   float far) {
  float temp, temp2, temp3, temp4;
  temp = 2.0 * near;
  temp2 = right - left;
  temp3 = top - bottom;
  temp4 = far - near;

  data[0] = temp / temp2;
  data[1] = 0.0;
  data[2] = 0.0;
  data[3] = 0.0;
  data[4] = 0.0;
  data[5] = temp / temp3;
  data[6] = 0.0;
  data[7] = 0.0;
  data[8] = (right + left) / temp2;
  data[9] = (top + bottom) / temp3;
  data[10] = (-far - near) / temp4;
  data[11] = -1.0;
  data[12] = 0.0;
  data[13] = 0.0;
  data[14] = (-temp * far) / temp4;
  data[15] = 0.0;
}

void Mat4::lookAt(Vector3f position, Vector3f target, Vector3f up) {
  Vector3f forward = position - target;
  forward.normalize();
  Vector3f right = up.cross(forward);
  right.normalize();
  Vector3f up_corrige = forward.cross(right);
  up_corrige.normalize();

  float r1 = position.dot(right);
  float r2 = position.dot(up_corrige);
  float r3 = position.dot(forward);

  data[0] = right.x;
  data[1] = up_corrige.x;
  data[2] = forward.x;
  data[3] = 0.0;

  data[4] = right.y;
  data[5] = up_corrige.y;
  data[6] = forward.y;
  data[7] = 0.0;

  data[8] = right.z;
  data[9] = up_corrige.z;
  data[10] = forward.z;
  data[11] = 0.0;

  data[12] = -r1;
  data[13] = -r2;
  data[14] = -r3;
  data[15] = 1.0;
}
