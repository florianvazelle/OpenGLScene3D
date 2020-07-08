#include "GLShader.h"
#include <GL/glew.h>

class Skybox {
public:
  void load_cube_map_sprite_texture(GLuint *_texId, const char *path_sprite[6]);
  void Init();
  void Draw();
  void Destroy();

private:
  GLShader g_SkyboxShader;
  GLuint _spaceTexId;
  GLuint skyboxVAO;
};