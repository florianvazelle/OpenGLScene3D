#version 330 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_proj3DMatrix;

void main() {
  mat4 modelViewMatrix = u_viewMatrix * u_modelMatrix;
  mat4 projectionMatrix = u_proj3DMatrix;

  gl_Position = projectionMatrix * modelViewMatrix * vec4(a_position, 1.0);
}