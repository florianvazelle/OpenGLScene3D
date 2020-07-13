#version 330 core
out vec4 FragColor;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform sampler2D shadowMap;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specColor;
uniform float shininess;

in vec3 v_normal;
in vec3 v_modPos;
in vec4 ShadowCoord;

const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightPower = 40.0;
const float screenGamma = 2.2;

vec3 diffuse(vec3 N, vec3 L) {
  vec3 nDir = normalize(N);
  vec3 lDir = normalize(L);

  return vec3(1.0, 0.0, 1.0) * diffuseColor * max(0.0, dot(nDir, lDir));
}

vec3 specular(vec3 N, vec3 L) {
  vec3 V = normalize(-v_modPos);

  // blinn-phong
  vec3 H = normalize(L + V);
  return vec3(1.0, 0.2, 0.3) * specColor * max(pow(dot(N, H), shininess), 0.0);
}

void main() {

  vec3 N = normalize(v_normal);
  vec3 L = lightPos - v_modPos;

  float bias = 0.005 * tan(acos(dot(N, L)));
  bias = clamp(bias, 0, 0.01);

  float visibility = 1.0;
  if (texture(shadowMap, ShadowCoord.xy).z < ShadowCoord.z - bias) {
    visibility = 0.5;
  }

  float distance = length(L);
  distance = distance * distance;
  L = normalize(L);

  vec3 colorLinear =
      ambientColor +
      (diffuse(N, L) * lightColor * lightPower / distance) * visibility +
      (specular(N, L) * lightColor * lightPower / distance) * visibility;
  vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / screenGamma));
  FragColor = vec4(colorGammaCorrected, 1.0);
}
