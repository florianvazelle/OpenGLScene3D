#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLShader.h"

class FrameBufferObject {
    private:
        GLuint FBO, m_colorbuffer, m_depthbuffer;
        GLuint VAO, VBO;
        GLShader g_TextureShader;
    
    public:
        void Init(GLFWwindow* window);
        void InitColorBuffer();
        void DrawColorBuffer(int width, int height);
        void Destroy();

        GLuint getID(void) const { return FBO; }
};