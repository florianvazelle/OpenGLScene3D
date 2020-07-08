#include "Mat4.h"
#include "Vector3f.h"

class GLCamera {
private:
  float m_theta;
  float m_phi;
  float m_radius;

  const int width = 1079, height = 940;
  const float transScale = 0.5f;

  float lastX, lastY;
  float yaw, pitch;

  Vector3f m_focus;
  Vector3f eye, lookat;

public:
  GLCamera();

  void rotate(float x, float y);
  void change(float x, float y);
  void zoom(float distance);
  Mat4 viewMatrix(void);
  void focus(Vector3f);
};