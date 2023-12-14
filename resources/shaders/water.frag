#version 330 core

in vec3 wp;
in vec3 wn;
in vec4 ClipSpace;
in vec2 uv;
in mat3 tinv_m;

out vec4 fragColor;

uniform sampler2D textureSampler1;
uniform sampler2D textureSampler2;
uniform sampler2D textureSampler_dudv;
uniform sampler2D textureSampler_normal;
uniform float time_step;

uniform float k_a;
uniform float k_d;
uniform float k_s;
uniform float shininess;
uniform vec4 camera_pos;

uniform vec4 cAmbient;
uniform vec4 cDiffuse;
uniform vec4 cSpecular;

struct Light
{
  int type;
  vec4 color;
  vec3 func;
  vec4 pos;
  vec4 dir;
  float penumbra;
  float angle;
};

uniform Light myL;

const float reflectivity = 0.6;
const float w_shininess = 100.0;

vec4 phong_dir_light(Light frag_light, vec3 N, vec3 V)
{
    vec3 L = -normalize(vec3(frag_light.dir));
    // specular shading
    vec3 halfwayDir = normalize(L + V);
    float specular = pow(max(dot(N, halfwayDir), 0.0), w_shininess);
    return specular*frag_light.color;
}


vec4 phong(vec3 N, vec3 V) {
    vec4 tmp_fragColor = phong_dir_light(myL, N, V);
    return min(tmp_fragColor, vec4(1.0));
}

void main()
{
    vec2 ndc = (ClipSpace.xy / ClipSpace.w) / 2.0 + 0.5;
    vec2 refraction_uv = vec2(ndc.x, ndc.y);
    vec2 reflection_uv = vec2(ndc.x, -ndc.y);

    vec2 distortion_coord = texture(textureSampler_dudv, vec2(uv.x+time_step, uv.y)).rg*0.1;
    distortion_coord = uv + vec2(distortion_coord.x, distortion_coord.y+time_step);
    vec2 distortion = (texture(textureSampler_dudv, distortion_coord).rg*2.0-1.0)*0.02;
    refraction_uv += distortion;
    refraction_uv = clamp(refraction_uv, 0.001, 0.999);
    reflection_uv += distortion;
    reflection_uv.x = clamp(reflection_uv.x, 0.001, 0.999);
    reflection_uv.y = clamp(reflection_uv.y, -0.999, -0.001);

    vec4 refractionColor = texture(textureSampler1, refraction_uv);
    vec4 reflectionColor = texture(textureSampler2, reflection_uv);

    vec3 V = normalize(vec3(camera_pos)-wp);
    fragColor = mix(reflectionColor, refractionColor, pow(dot(V, vec3(0.0, 1.0, 0.0)), reflectivity));
    vec4 water_color = vec4(0.0, 0.3, 0.5, 1.0);
    fragColor = mix(fragColor, water_color, 0.2);

    vec4 normal_color = texture(textureSampler_normal, distortion_coord);
    vec3 N = normalize(vec3(normal_color.r*2.0-1.0, normal_color.b, normal_color.g*2.0-1.0));
    N = normalize(tinv_m * N);
    vec4 phong_color = phong(N, V);

    fragColor = min(fragColor + reflectivity*phong(N, V), vec4(1.0));
}
