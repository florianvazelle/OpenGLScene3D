attribute vec3 vsiPosition;

uniform mat4 u_viewMatrix;
uniform mat4 u_proj3DMatrix;

varying vec3 TexCoords;

void main() {
  vec4 pos = /* u_proj3DMatrix * u_viewMatrix * */vec4(vsiPosition, 1.0);
  gl_Position = pos.xyww;
  TexCoords = vsiPosition;
}
