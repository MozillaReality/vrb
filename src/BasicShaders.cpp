namespace vrb {

static const char* sVertexShaderSource = R"SHADER(

#define MAX_LIGHTS 2
#define VRB_USE_TEXTURE VRB_TEXTURE_STATE
#define VRB_UV_TYPE VRB_TEXTURE_UV_TYPE
#define VRB_UV_TRANSFORM VRB_UV_TRANSFORM_ENABLED
#define VRB_VERTEX_COLOR VRB_VERTEX_COLOR_ENABLED

struct Light {
  vec3 direction;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
};

struct Material {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float specularExponent;
};

uniform mat4 u_perspective;
uniform mat4 u_view;
uniform mat4 u_model;
uniform int u_lightCount;
uniform Light u_lights[MAX_LIGHTS];
uniform Material u_material;
uniform vec4 u_tintColor;
#if VRB_UV_TRANSFORM == 1
uniform mat4 u_uv_transform;
#endif

attribute vec3 a_position;
attribute vec3 a_normal;

varying vec4 v_color;

#ifdef VRB_USE_TEXTURE
attribute VRB_UV_TYPE a_uv;
varying VRB_UV_TYPE v_uv;
#endif // VRB_USE_TEXTURE

#if VRB_VERTEX_COLOR == 1
attribute vec4 a_color;
#endif

vec4 normal;

vec4
calculate_light(int index) {
  vec4 result = vec4(0, 0, 0, 0);
  vec4 direction = -normalize(u_view * vec4(u_lights[index].direction.xyz, 0));
  vec4 hvec;
  float ndotl;
  float ndoth;
  result += u_lights[index].ambient * u_material.ambient;
  ndotl = max(0.0, dot(normal, direction));
  result += (ndotl * u_lights[index].diffuse * u_material.diffuse);
  hvec = normalize(direction + vec4(0.0, 0.0, 1.0, 0.0));
  ndoth = dot(normal, hvec);
  if (ndoth > 0.0) {
    result += (pow(ndoth, u_material.specularExponent) * u_material.specular * u_lights[index].specular);
  }
  return result;
}

void main(void) {
  int ix;
  v_color = vec4(0, 0, 0, 0);
  normal = normalize(u_view * u_model * vec4(a_normal.xyz, 0));
  for(ix = 0; ix < MAX_LIGHTS; ix++) {
    if (ix >= u_lightCount) {
      break;
    }
    v_color += calculate_light(ix);
    v_color.a = u_material.diffuse.a;
  }
  if (u_lightCount == 0) {
    v_color = u_material.diffuse;
  }
#if VRB_VERTEX_COLOR == 1
  v_color *= a_color;
#endif
  v_color *= u_tintColor;
#ifdef VRB_USE_TEXTURE
#if VRB_UV_TRANSFORM == 1
  v_uv = (u_uv_transform * vec4(a_uv.xy, 0, 1)).xy;
#else
  v_uv = a_uv;
#endif // VRB_UV_TRANSFORM
#endif // VRB_USE_TEXTURE
  gl_Position = u_perspective * u_view * u_model * vec4(a_position.xyz, 1);
}

)SHADER";

static const char* sFragmentShaderSource = R"SHADER(
precision VRB_FRAGMENT_PRECISION float;

varying vec4 v_color;

void main() {
  gl_FragColor = v_color;
}

)SHADER";

static const char* sFragmentTextureShaderSource = R"SHADER(
precision VRB_FRAGMENT_PRECISION float;

uniform sampler2D u_texture0;
varying vec4 v_color;
varying vec2 v_uv;

void main() {
  gl_FragColor = texture2D(u_texture0, v_uv) * v_color;
}

)SHADER";

static const char* sFragmentSurfaceTextureShaderSource = R"SHADER(
#extension GL_OES_EGL_image_external : require
precision VRB_FRAGMENT_PRECISION float;

uniform samplerExternalOES u_texture0;
varying vec4 v_color;
varying vec2 v_uv;

void main() {
  gl_FragColor = texture2D(u_texture0, v_uv) * v_color;
}

)SHADER";

static const char* sFragmentCubeMapTextureShaderSource = R"SHADER(
precision VRB_FRAGMENT_PRECISION float;

uniform samplerCube u_texture0;
varying vec4 v_color;
varying vec3 v_uv;

void main() {
  gl_FragColor = textureCube(u_texture0, v_uv) * v_color;
}

)SHADER";

const char*
GetVertexShaderSource() { return sVertexShaderSource; }

const char*
GetFragmentShaderSource() { return sFragmentShaderSource; }

const char*
GetFragmentTextureShaderSource() { return sFragmentTextureShaderSource; }

const char*
GetFragmentSurfaceTextureShaderSource() { return sFragmentSurfaceTextureShaderSource; }

const char*
GetFragmentCubeMapTextureShaderSource() { return sFragmentCubeMapTextureShaderSource; }

} // namespace vrb
