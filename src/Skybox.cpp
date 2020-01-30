#include "Skybox.h"
#include "Utils.h"

void Skybox::load_cube_map_sprite_texture(GLuint *_texId, const char *path_sprite[6]) {
    if (*_texId == 0) {
		int w, h, c;
        uint8_t* xpos = Load(path_sprite[0], w, h, c);
        uint8_t* xneg = Load(path_sprite[1], w, h, c);
        uint8_t* ypos = Load(path_sprite[2], w, h, c);
        uint8_t* yneg = Load(path_sprite[3], w, h, c);
        uint8_t* zpos = Load(path_sprite[4], w, h, c);
        uint8_t* zneg = Load(path_sprite[5], w, h, c);

		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_CUBE_MAP);

		glGenTextures(1, _texId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, *_texId);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, xpos);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, xneg);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ypos);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, yneg);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, zpos);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, zneg);

		Free(xneg);
		Free(xpos);
		Free(yneg);
		Free(ypos);
		Free(zneg);
		Free(zpos);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Skybox::Init() {
        /* Chargement des shaders pour la skybox */
		g_SkyboxShader.LoadVertexShader("./assets/shaders/Skybox.vs");
		g_SkyboxShader.LoadFragmentShader("./assets/shaders/Skybox.fs");
		g_SkyboxShader.Create();

        /* Chargement de la texture cube */
        const char *skybox_path_sprite[6] = {
                "assets/textures/skybox/rigth.png", "assets/textures/skybox/left.png",
                "assets/textures/skybox/top.png",   "assets/textures/skybox/bottom.png",
                "assets/textures/skybox/front.png", "assets/textures/skybox/back.png"
        };
        load_cube_map_sprite_texture(&_spaceTexId, skybox_path_sprite);

        GLfloat skyboxVertices[] = {
                -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
                1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

                -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
                -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

                1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

                -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

                -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

                -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
                1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f
        };

        GLuint skyboxVBO = 0;

        /* Configuration de la skybox VAO */
        glGenVertexArrays(1, &skyboxVAO);
        glBindVertexArray(skyboxVAO);

        glGenBuffers(1, &skyboxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        
		glEnableVertexAttribArray(0);
        
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
        glBindVertexArray(0);
}

void Skybox::Draw() {
        glDepthFunc(GL_LEQUAL);

		auto shader = g_SkyboxShader.GetProgram();
        glUseProgram(shader);

        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _spaceTexId);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glUseProgram(0);
}

void Skybox::Destroy() {
    g_SkyboxShader.Destroy();
}
