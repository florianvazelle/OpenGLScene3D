#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_proj3DMatrix;
uniform mat4 u_depthMVP;

out vec3 v_normal;
out vec3 v_modPos;
out vec4 ShadowCoord;

void main(void) {
  mat4 modelViewMatrix = u_modelMatrix;
  mat4 projectionMatrix = u_proj3DMatrix;

  vec3 mp = vec3(u_modelMatrix * vec4(a_position, 1.0));
  v_normal = transpose(inverse(mat3(u_modelMatrix))) * a_normal;
  v_modPos = mp.xyz;
  gl_Position = projectionMatrix * u_viewMatrix * vec4(mp, 1);

  ShadowCoord = u_depthMVP * vec4(mp, 1);
}
