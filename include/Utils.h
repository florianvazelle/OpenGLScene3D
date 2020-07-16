#include <GL/glew.h>

uint8_t *Load(const char *path, int &w, int &h, int &c);
void Free(uint8_t *data);
void _check_gl_error(const char *file, int line);