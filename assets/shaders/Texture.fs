
varying vec2 v_texcoords;
uniform sampler2D u_TextureSampler;

void main(void) { gl_FragColor = texture2D(u_TextureSampler, v_texcoords); }