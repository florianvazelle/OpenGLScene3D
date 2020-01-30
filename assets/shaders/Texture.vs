
attribute vec2 a_position;
attribute vec2 a_texcoords;

uniform mat4 u_modelMatrix;

varying vec2 v_texcoords;

void main(void) {
	v_texcoords = a_texcoords;
	gl_Position = u_modelMatrix * vec4(a_position, 0.0, 1.0);
}