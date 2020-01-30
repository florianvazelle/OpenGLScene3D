
varying vec2 v_texcoords;
uniform sampler2D u_TextureSampler;
uniform vec2 dimensions;

void main(void) {
	bool useBlackWhite = false;
	bool useSepia = false;
	bool useConvolution = false;

	vec4 texColor = texture2D(u_TextureSampler, v_texcoords);
	
	// Noir et blanc
	if (useBlackWhite) {
		float moyenne = (texColor.r + texColor.g + texColor.b) / 3.0;
		vec4 color = (moyenne < 0.5) ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);	
	} else

	// Sepia
	if (useSepia) {
		vec4 sepia = vec4(
			dot(texColor.rgb, vec3(0.393, 0.769, 0.189)),
			dot(texColor.rgb, vec3(0.349, 0.686, 0.168)),
			dot(texColor.rgb, vec3(0.272, 0.534, 0.131)),
			1.0
		);

		gl_FragColor = sepia;
	} else

	if (useConvolution) {
		float right = 1.0 / dimensions[0];
		float left = -1.0 / dimensions[0];

		float top = 1.0 / dimensions[1];
		float down = -1.0 / dimensions[1];

		vec4 tex_voisins[9];

		tex_voisins[0] = texture2D(u_TextureSampler, v_texcoords + vec2(right, top));
		tex_voisins[1] = texture2D(u_TextureSampler, v_texcoords + vec2(0.0, top));
		tex_voisins[2] = texture2D(u_TextureSampler, v_texcoords + vec2(left, top));

		tex_voisins[3] = texture2D(u_TextureSampler, v_texcoords + vec2(right, 0.0));
		tex_voisins[4] = texture2D(u_TextureSampler, v_texcoords + vec2(0.0, 0.0));
		tex_voisins[5] = texture2D(u_TextureSampler, v_texcoords + vec2(left, 0.0));

		tex_voisins[6] = texture2D(u_TextureSampler, v_texcoords + vec2(right, down));
		tex_voisins[7] = texture2D(u_TextureSampler, v_texcoords + vec2(0.0, down));
		tex_voisins[8] = texture2D(u_TextureSampler, v_texcoords + vec2(left, down));

		mat3 matrix = mat3(0, -1, 0, -1, 5, -1, 0, -1, 0);

		vec4 moy = vec4(0);
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++) {
				moy += tex_voisins[i * 3 + j] * matrix[i][j];
			}
		}

		gl_FragColor = moy;
	} else {
		// Sobel
		float right = 1.0 / dimensions[0];
		float left = -1.0 / dimensions[0];

		float top = 1.0 / dimensions[1];
		float down = -1.0 / dimensions[1];

		vec4 tex_voisins[9];

		tex_voisins[0] = texture2D(u_TextureSampler, v_texcoords + vec2(right, top));
		tex_voisins[1] = texture2D(u_TextureSampler, v_texcoords + vec2(0.0, top));
		tex_voisins[2] = texture2D(u_TextureSampler, v_texcoords + vec2(left, top));

		tex_voisins[3] = texture2D(u_TextureSampler, v_texcoords + vec2(right, 0.0));
		tex_voisins[4] = texture2D(u_TextureSampler, v_texcoords + vec2(0.0, 0.0));
		tex_voisins[5] = texture2D(u_TextureSampler, v_texcoords + vec2(left, 0.0));

		tex_voisins[6] = texture2D(u_TextureSampler, v_texcoords + vec2(right, down));
		tex_voisins[7] = texture2D(u_TextureSampler, v_texcoords + vec2(0.0, down));
		tex_voisins[8] = texture2D(u_TextureSampler, v_texcoords + vec2(left, down));

		mat3 gx = mat3(-1, 0, 1, -2, 0, 2, -1, 0, 1);
		mat3 gy = mat3(-1, -2, -1, 0, 0, 0, 1, 2, 1);

		vec4 moyx = vec4(0);
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++) {
				moyx += tex_voisins[i * 3 + j] * gx[i][j];
			}
		}

		vec4 moyy = vec4(0);
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++) {
				moyy += tex_voisins[i * 3 + j] * gy[i][j];
			}
		}

		gl_FragColor = sqrt(moyx * moyx + moyy * moyy);
	}
}