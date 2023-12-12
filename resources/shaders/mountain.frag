#version 330 core
in vec4 vert;
in vec4 norm;
in vec3 color;
in vec3 lightDir;

uniform bool wireshade;

uniform sampler2D rockSampler;
uniform sampler2D grassSampler;

out vec4 fragColor;

void main(void)
{
    fragColor = vec4(0.f);
    if (wireshade) {
        fragColor = vec4(color,1);
    } else {
        vec3 objColor = color;
        vec4 lightColor = vec4(1, 237.f/255.f, 219.f/255.f, 1);

        float dot_n_li = clamp(dot(norm.xyz, lightDir), 0, 1);


        vec4 ambient = vec4((clamp(dot(norm.xyz, lightDir), 0, 1) * 0.7 +  0.3) * objColor, 1.0);
        fragColor += ambient;

        vec2 textureCoord = vert.xy;

        vec4 textureColor = texture(rockSampler, textureCoord);

        vec4 diffuse = lightColor * dot_n_li;
        float kd = 0.7f;
        diffuse = textureColor * (1 - kd) + diffuse * kd;
        fragColor += diffuse;
    }
}
