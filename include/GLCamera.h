#include "Mat4.h"
#include "Vector3f.h"

class GLCamera {
    private:
        float m_theta, m_theta2;
        float m_phi, m_phi2;
        float m_radius;

        float lastX, lastY;
        float yaw, pitch;

        Vector3f m_focus;

    public:
        GLCamera();

        void rotate(float x, float y);
        void change(float x, float y);
        void zoom(float distance);
        Mat4 viewMatrix(void);
        void focus(Vector3f);
};