attribute vec3 vsiPosition;

varying vec3 TexCoords;

void main() {
  vec4 pos = vec4(vsiPosition, 1.0);
  gl_Position = pos.xyww;
  TexCoords = vsiPosition;
}
