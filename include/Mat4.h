#ifndef MAT_H
#define MAT_H

#include "Vector3f.h"

class Mat4 {
    public:
        Mat4();
        Mat4(const Mat4& m);

        Mat4 operator*(const Mat4& m);
        Vector3f operator*(const Vector3f& v) {
            Vector3f tmp;
            tmp.x =  v.x * data[0] + v.y * data[4] + v.z * data[8]; 
            tmp.y =  v.x * data[1] + v.y * data[5] + v.z * data[9]; 
            tmp.z =  v.x * data[2] + v.y * data[6] + v.z * data[10]; 
            return tmp;
        }

        float& operator[](int i);
        const float& operator[](int i) const;

        void identity(void);
        void display(void);
        void scale(float, float, float);
        void translate(float, float, float);
        void rotateX(float);
        void rotateY(float);
        void rotateZ(float);
        void ortho(float, float, float, float, float, float);
        void proj3D(float, float, float, float);
        void makePerspective(float, float, float, float);
        void frustum(float, float, float, float, float, float);

        void lookAt(Vector3f, Vector3f, Vector3f);

    private:
        float data[16];

};

#endif