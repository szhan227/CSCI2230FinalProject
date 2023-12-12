#version 330 core

in vec3 fragPos;

out vec4 fragColor;

uniform vec4 lightDir;

uniform vec4 cameraPosition;

const float PI = 3.1415926;
const float iStepNum = 16.0;
const float jStepNum = 8.0;
const float cloudScale = 0.003;    // Adjust this value based on testing
const vec3 cloudColor = vec3(0.8, 0.8, 0.8);

vec2 ray_sphere_intersection(vec3 ori, vec3 dir, float radius) {
    float a = dot(dir, dir);
    float b = 2.0 * dot(dir, ori);
    float c = dot(ori, ori) - (radius * radius);
    float b24ac = (b*b) - 4.0*a*c;
    if (b24ac < 0.0)
        return vec2(1e-5, -1e-5);
    return vec2(
        (-b - sqrt(b24ac))/(2.0*a),
        (-b + sqrt(b24ac))/(2.0*a)
    );
}

vec3 sky(vec3 viewDir, vec3 cameraOrigin, vec3 sunDir, float sunIntensity, float earthRadius, float atomsphereRadius, vec3 kRayleigh, float kMie, float rayleighHeight, float mieHeight, float g) {
    sunDir = normalize(sunDir);
    viewDir = normalize(viewDir);

    // 3 cases in total
    vec2 atomsphere_t = ray_sphere_intersection(cameraOrigin, viewDir, atomsphereRadius);
    vec2 earth_t = ray_sphere_intersection(cameraOrigin, viewDir, earthRadius);
    vec2 t;
    if (earth_t.x > earth_t.y)
        t = atomsphere_t;
    else if (atomsphere_t.x < 0)
        t = vec2(0, atomsphere_t.y);
    else
        t = vec2(atomsphere_t.x, earth_t.x);

    float iStepSize = (t.y - t.x) / iStepNum;

    float iTime = 0.0;

    vec3 totalRlh = vec3(0,0,0);
    vec3 totalMie = vec3(0,0,0);
    vec3 totalCloud = vec3(0,0,0);
    float totalCloudDensity = 0.0;

    float iOdRlh = 0.0;
    float iOdMie = 0.0;

    float mu = dot(viewDir, sunDir);
    float mumu = mu * mu;
    float gg = g * g;
    float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

    for (int i = 0; i < iStepNum; i++) {
        vec3 iPos = cameraOrigin + viewDir * (iTime + iStepSize * 0.5);

        float iHeight = length(iPos) - earthRadius;


        float odStepRlh = exp(-iHeight / rayleighHeight) * iStepSize;
        float odStepMie = exp(-iHeight / mieHeight) * iStepSize;

        iOdRlh += odStepRlh;
        iOdMie += odStepMie;

        float jStepSize = ray_sphere_intersection(iPos, sunDir, atomsphereRadius).y / float(jStepNum);

        float jTime = 0.0;

        float jOdRlh = 0.0;
        float jOdMie = 0.0;

        for (int j = 0; j < jStepNum; j++) {
            vec3 jPos = iPos + sunDir * (jTime + jStepSize * 0.5);

            float jHeight = length(jPos) - earthRadius;

            jOdRlh += exp(-jHeight / rayleighHeight) * jStepSize;
            jOdMie += exp(-jHeight / mieHeight) * jStepSize;

            jTime += jStepSize;
        }


        vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRayleigh * (iOdRlh + jOdRlh)));

        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;

        iTime += iStepSize;

    }

    return sunIntensity * (pRlh * kRayleigh * totalRlh + pMie * kMie * totalMie);
}

void main() {
    float earthRadius = 6.36e6;
    float atomsphereRadius = 6.42e6;
    vec3 cameraOrigin = vec3(0, earthRadius, 0) + cameraPosition.xyz;
    float sunIntensity = 22.0;
    vec3 kRayleigh = vec3(5.31e-6, 13.5e-6, 33.1e-6);
    float kMie = 4e-6;
    float rayleighHeight = 8e3;
    float mieHeight = 1.2e3;
    float g = 0.98;

    vec3 color = sky(
        normalize(fragPos),
        cameraOrigin,
        normalize(lightDir.xyz),
        sunIntensity,
        earthRadius,
        atomsphereRadius,
        kRayleigh,
        kMie,
        rayleighHeight,
        mieHeight,
        g
    );

    color = 1.0 - exp(-2.0 * color);

    fragColor = vec4(color, 1);
}

