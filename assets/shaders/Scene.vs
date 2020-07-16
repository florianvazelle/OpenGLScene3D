#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 3) in vec2 a_texcoord;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_proj3DMatrix;

out vec3 v_normal;
out vec2 v_texcoord;
out vec3 v_modPos;

void main(void) {
  mat4 modelViewMatrix = u_viewMatrix * u_modelMatrix;
  mat4 projectionMatrix = u_proj3DMatrix;

  vec4 mp = modelViewMatrix * vec4(a_position, 1.0);
  v_normal = (transpose(inverse(modelViewMatrix)) * vec4(a_normal, 0.0)).xyz;
  v_modPos = mp.xyz;
  v_texcoord = a_texcoord;
  gl_Position = projectionMatrix * mp;
}
