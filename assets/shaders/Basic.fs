uniform int u_hasTex;

uniform vec3 lightPos;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specColor;
uniform float shininess;
uniform sampler2D u_TextureSampler;

varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_modPos;

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
  if (u_hasTex == 1) {
    gl_FragColor = texture2D(u_TextureSampler, v_texcoord);
  } else if (u_hasTex == 2) {
    gl_FragColor = vec4(v_normal, 1.0);
  } else {
    vec3 N = normalize(v_normal);
    vec3 L = lightPos - v_modPos;
    float distance = length(L);
    distance = distance * distance;
    L = normalize(L);

    vec3 colorLinear =
        ambientColor +
        diffuse(N, L) * lightColor * lightPower / distance +
        specular(N, L) * lightColor * lightPower / distance;
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / screenGamma));
    gl_FragColor = vec4(colorGammaCorrected, 1.0);
  }
}
