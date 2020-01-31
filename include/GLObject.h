#include <GL/glew.h>
#include <map>
#include <string>

#include "GLShader.h"

class GLObject {
    public:
        void LoadObject(const char* filename, GLShader g_BasicShader);
        void DrawObject(void);
        void DestroyObject(void);
        GLuint GetObject(void) const { return m_vao; };

        struct Vertex {
            float x, y, z;
            float nx, ny, nz;
            float texX, texY;
            GLushort idx;

            bool operator==(const Vertex& v) const {
                return this->x == v.x && this->y == v.y && this->z == v.z
                    && this->nx == v.nx && this->ny == v.ny && this->nz == v.nz;
            }
        };

        struct Material {
            float ambient[3];
            float diffuse[3];
            float specular[3];
            float shininess;
            std::string diffuse_texname;
        };

    private:
        GLuint m_vao, m_vbo, m_ibo;
        uint32_t shader;
        std::vector<GLushort> indices;
        std::vector<Material> mats;
        std::map<std::string, GLuint> textures;
};