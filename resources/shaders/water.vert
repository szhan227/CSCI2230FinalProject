#version 330 core

// op => object vertices position
// on => object vertices normal
layout(location = 0) in vec3 op;
layout(location = 1) in vec3 on;
// Task 5: declare `out` variables for the world-space position and normal,
//         to be passed to the fragment shader
out vec3 wp;
out vec3 wn;
out vec4 ClipSpace;
out vec2 uv;
out mat3 tinv_m;

// Task 6: declare a uniform mat4 to store model matrix
uniform mat4 m_Matrix;
// Task 7: declare uniform mat4's for the view and projection matrix
uniform mat4 v_Matrix, proj_Matrix;


void main() {
    wp = vec3(m_Matrix * vec4(op, 1.0));
    wn = normalize(transpose(inverse(mat3(m_Matrix))) * on);
    tinv_m = transpose(inverse(mat3(m_Matrix)));
    gl_Position = proj_Matrix * v_Matrix * m_Matrix * vec4(op, 1.0f);
    ClipSpace = gl_Position;
    uv = vec2(op.x/2.0+0.5, op.z/2.0+0.5)*2.0;
}
