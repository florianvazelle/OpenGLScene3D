uniform vec3 cameraPos;
uniform samplerCube skybox;

varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_modPos;

void main() {
  vec3 I = normalize(v_modPos - cameraPos);
  vec3 R = reflect(I, normalize(v_normal));
  gl_FragColor = vec4(textureCube(skybox, R).rgb, 1.0);
}
