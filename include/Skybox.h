#include "GLShader.h"
#include "Mat4.h"
#include <GL/glew.h>

class Skybox {
public:
  GLuint load_cube_map_sprite_texture(const char *path_sprite[6]);
  void Init();
  void Draw(Mat4, Mat4);
  void Destroy();
  static void _check_gl_error(const char *file, int line);
  GLuint getSpaceTexId() const { return _spaceTexId; };

private:
  GLShader g_SkyboxShader;
  GLuint _spaceTexId;
  GLuint skyboxVAO;
};