#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

class Vector3f {
public:
  float x, y, z;

  Vector3f operator*(const int i) { return {x * i, y * i, z * i}; }

  Vector3f operator+(const Vector3f &v) { return {x * v.x, y * v.y, z * v.z}; }

  Vector3f operator-() { return {-x, -y, -z}; }

  Vector3f operator-(const Vector3f &o) { return {x - o.x, y - o.y, z - o.z}; }

  float dot(Vector3f v) { return x * v.x + y * v.y + z * v.z; }

  Vector3f cross(Vector3f v) const {
    return {(y * v.z - z * v.y), -(x * v.z - z * v.x), (x * v.y - y * v.x)};
  }

  float norm(void) { return sqrt((x * x) + (y * y) + (z * z)); }
  Vector3f &normalize() {
    float n = 1 / norm();

    x *= n;
    y *= n;
    z *= n;
    return *this;
  }
};

#endif