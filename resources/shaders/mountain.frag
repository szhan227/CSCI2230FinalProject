#version 330 core
in vec4 vert;
in vec4 norm;
in vec3 color;
in vec3 lightDir;

uniform bool wireshade;

uniform sampler2D rockSampler;
uniform sampler2D grassSampler;
uniform float kd;
uniform float ka;

uniform int season;

out vec4 fragColor;

void main(void)
{
    fragColor = vec4(0.f);
    if (wireshade) {
        fragColor = vec4(color,1);
    } else {
        vec3 objColor = color;
        vec3 upRight = vec3(0, 0, 1);
        if (season == 0) {
            objColor = objColor;
        } else if (season == 1) {
            if ((vert[1] > 0.2 && vert[1] < 0.75) && (acos(dot(vec3(norm), upRight) / length(vec3(norm)) / length(upRight)) < 0.62)) {
                objColor = vec3(0, 0.3, 0);
            }
        } else if (season == 2) {
            // vec4 fallYellow = vec4(1.0, 0.85, 0.2, 1.0);
            // float blendFactor = 1.0 - fragColor.b; 
            // blendFactor = mix(0.1, 0.2, blendFactor);
            // fragColor.r = mix(fragColor.r, fallYellow.r, blendFactor);
            // fragColor.g = mix(fragColor.g, fallYellow.g, blendFactor);
            // fragColor.b = mix(fragColor.b, fallYellow.b, blendFactor);
            // vec3 diff_vector = objColor - vec3(0.5f, 0.25f, 0.f);
        } else if (season == 3) {
            // white
            // if (acos(dot(norm, upRight) / length(norm) / length(upRight))) 
            if (vert[1] > 0.5 || (acos(dot(vec3(norm), upRight) / length(vec3(norm)) / length(upRight)) < 0.628)) {
                float eps = clamp(5.f - vert[2], 0.f, 4.f) / 4.f;
                objColor = vec3(1.0,1.0,1.0) * eps + objColor * (1 - eps);
            }  
        }
        vec4 lightColor = vec4(1, 237.f/255.f, 219.f/255.f, 1);

        float dot_n_li = clamp(dot(norm.xyz, lightDir), 0, 1);


        // vec4 ambient = vec4((clamp(dot(norm.xyz, lightDir), 0, 1) * 0.7 +  0.3) * objColor, 1.0);
        vec4 ambient = vec4((clamp(dot(norm.xyz, lightDir), 0, 1) * ka +  (1-ka)) * objColor, 1.0);
        fragColor += ambient;

        vec2 textureCoord = vert.xy;

        vec4 textureColor = texture(rockSampler, textureCoord * 2);

        vec4 diffuse = lightColor * dot_n_li;
        // float kd = 0.7f;
        diffuse = textureColor * (1 - kd) + diffuse * kd;
        fragColor += diffuse;
    }
}
