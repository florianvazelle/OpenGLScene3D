#version 330 core
out vec4 FragColor;

uniform int u_type;

uniform vec3 lightPos;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specColor;
uniform float shininess;
uniform sampler2D u_TextureSampler;

in vec3 v_normal;
in vec2 v_texcoord;
in vec3 v_modPos;

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
  if (u_type == 1) {
    const vec4 lum_diffus = vec4(1, 1, 0.9, 1);
    const vec4 lum_amb = vec4(0.8, 0.8, 1, 1);
    const vec4 lum_spec = vec4(1, 1, 0.75, 1);
    const float Iamb = 0.15;

    vec3 L = normalize(v_modPos - lightPos.xyz);
    float Idiffuse = 0, Ispec = 0;
    vec4 color = vec4(1);
    vec3 N = normalize(v_normal);

    vec3 B = cross(normalize(vec3(N.x, 0, N.z)), vec3(0, 1, 0));
    vec3 T = cross(N, B);
    Idiffuse = clamp(dot(N, -L), 0, 1);

    color = texture2D(u_TextureSampler, v_texcoord);
    FragColor = lum_diffus * color * Idiffuse + lum_amb * Iamb * color +
                lum_spec * Ispec;
  } else if (u_type == 2) {
    FragColor = vec4(v_normal, 1.0);
  } else {
    vec3 N = normalize(v_normal);
    vec3 L = lightPos - v_modPos;
    float distance = length(L);
    distance = distance * distance;
    L = normalize(L);

    vec3 colorLinear = ambientColor +
                       diffuse(N, L) * lightColor * lightPower / distance +
                       specular(N, L) * lightColor * lightPower / distance;
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / screenGamma));
    FragColor = vec4(colorGammaCorrected, 1.0);
  }
}