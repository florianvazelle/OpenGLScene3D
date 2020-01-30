#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

uint8_t* Load(const char* path, int& w, int& h, int& c) {
	return stbi_load(path, &w, &h, &c, STBI_rgb_alpha);
}

void Free(uint8_t* data) {
	stbi_image_free(data);
}