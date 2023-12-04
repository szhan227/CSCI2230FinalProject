#include "Mountaingenerator.h"

#include <cmath>
#include "glm/glm.hpp"
#include <iostream>

// Constructor
MountainGenerator::MountainGenerator()
{
    // Task 8: turn off wireframe shading
    //  m_wireshade = true; // STENCIL CODE
    m_wireshade = false; // TA SOLUTION

    // Define resolution of Mountain generation
    m_resolution = 500;

    // Generate random vector lookup table
    m_lookupSize = 1024;
    m_randVecLookup.reserve(m_lookupSize);

    // Initialize random number generator
    std::srand(1230);

    // Populate random vector lookup table
    for (int i = 0; i < m_lookupSize; i++)
    {
        m_randVecLookup.push_back(glm::vec2(std::rand() * 2.0 / RAND_MAX - 1.0,
                                            std::rand() * 2.0 / RAND_MAX - 1.0));
    }
}

// Destructor
MountainGenerator::~MountainGenerator()
{
    m_randVecLookup.clear();
}

// Helper for generateMountain()
void addPointToVector(glm::vec3 point, std::vector<float>& vector) {
    vector.push_back(point.x);
    vector.push_back(point.y);
    vector.push_back(point.z);
}

// Generates the geometry of the output triangle mesh
std::vector<float> MountainGenerator::generateMountain() {
    std::vector<float> verts;
    verts.reserve(m_resolution * m_resolution * 6);

    for(int x = 0; x < m_resolution; x++) {
        for(int y = 0; y < m_resolution; y++) {
            int x1 = x;
            int y1 = y;

            int x2 = x + 1;
            int y2 = y + 1;

            glm::vec3 p1 = getPosition(x1,y1);
            glm::vec3 p2 = getPosition(x2,y1);
            glm::vec3 p3 = getPosition(x2,y2);
            glm::vec3 p4 = getPosition(x1,y2);

            glm::vec3 n1 = getNormal(x1,y1);
            glm::vec3 n2 = getNormal(x2,y1);
            glm::vec3 n3 = getNormal(x2,y2);
            glm::vec3 n4 = getNormal(x1,y2);

            // tris 1
            // x1y1z1
            // x2y1z2
            // x2y2z3
            addPointToVector(p1, verts);
            addPointToVector(n1, verts);
            addPointToVector(getColor(n1, p1), verts);

            addPointToVector(p2, verts);
            addPointToVector(n2, verts);
            addPointToVector(getColor(n2, p2), verts);

            addPointToVector(p3, verts);
            addPointToVector(n3, verts);
            addPointToVector(getColor(n3, p3), verts);

            // tris 2
            // x1y1z1
            // x2y2z3
            // x1y2z4
            addPointToVector(p1, verts);
            addPointToVector(n1, verts);
            addPointToVector(getColor(n1, p1), verts);

            addPointToVector(p3, verts);
            addPointToVector(n3, verts);
            addPointToVector(getColor(n3, p3), verts);

            addPointToVector(p4, verts);
            addPointToVector(n4, verts);
            addPointToVector(getColor(n4, p4), verts);
        }
    }
    return verts;
}

// Samples the (infinite) random vector grid at (row, col)
glm::vec2 MountainGenerator::sampleRandomVector(int row, int col)
{
    std::hash<int> intHash;
    int index = intHash(row * 41 + col * 43) % m_lookupSize;
    return m_randVecLookup.at(index);
}

// Takes a grid coordinate (row, col), [0, m_resolution), which describes a vertex in a plane mesh
// Returns a normalized position (x, y, z); x and y in range from [0, 1), and z is obtained from getHeight()
glm::vec3 MountainGenerator::getPosition(int row, int col) {
    // Normalizing the planar coordinates to a unit square
    // makes scaling independent of sampling resolution.

    int scale = 10;
    float center = scale / 2.f;
    float x = 1.f * scale * row / m_resolution;
    float y = 1.f * scale * col / m_resolution;

    float z;
    z = getHeight(x, y);
    float distance = std::sqrt((x - center) * (x - center) + (y - center) * (y - center));
    if(distance <= 3.f){
        float coef = (distance * distance / 9.f);
        z *= coef;
    }
    return glm::vec3(x,y,z);
}

// ================== Students, please focus on the code below this point

// Helper for computePerlin() and, possibly, getColor()
float interpolate(float A, float B, float alpha) {
    // Task 4: implement your easing/interpolation function below
    float ease = 3 * alpha * alpha - 2 * alpha * alpha * alpha;
    float output = A + ease * (B - A);

    // Return 0 as placeholder
    return output;
}

// Takes a normalized (x, y) position, in range [0,1)
// Returns a height value, z, by sampling a noise function
float MountainGenerator::getHeight(float x, float y) {

    // input scale, the larger, the more frequent (number of peak and valley in a fixed area).
    // output scale, the larger, the more amplitude of the height and depth of peak nad valley.

    float frequency = 3.f;
    float amplitude = 1.f;
    // Task 6: modify this call to produce noise of a different frequency
    float z = 0.f;

    for(int i = 0; i < 4; i++){
        z += amplitude * computePerlin(x * frequency, y * frequency) / 2;
        frequency *= 2;
        amplitude /= 2;
    }
    // Task 7: combine multiple different octaves of noise to produce fractal perlin noise

    // Return 0 as placeholder
    return z;
}

// Computes the normal of a vertex by averaging neighbors
glm::vec3 MountainGenerator::getNormal(int row, int col) {
    // Task 9: Compute the average normal for the given input indices
    glm::vec3 normal(0, 0, 0);
    glm::vec3 V = getPosition(row, col);

    std::vector<std::vector<int>> neighborOffsets = { // Counter-clockwise around the vertex
        {-1, -1},
        { 0, -1},
        { 1, -1},
        { 1,  0},
        { 1,  1},
        { 0,  1},
        {-1,  1},
        {-1,  0}
    };

    for(int i = 0; i < 8; i++){
        int x1 = neighborOffsets[i][1] + col;
        int y1 = neighborOffsets[i][0] + row;
        int x2 = neighborOffsets[(i + 1) % 8][1] + col;
        int y2 = neighborOffsets[(i + 1) % 8][0] + row;
        glm::vec3 n1 = getPosition(y1, x1);
        glm::vec3 n2 = getPosition(y2, x2);
        normal += glm::normalize(glm::cross(n1 - V, n2 - V));
    }

    // Return up as placeholder
    return glm::normalize(normal);
}

// Computes color of vertex using normal and, optionally, position
glm::vec3 MountainGenerator::getColor(glm::vec3 normal, glm::vec3 position) {
    // Task 10: compute color as a function of the normal and position

    glm::vec3 white(1, 1, 1);
    glm::vec3 gray(0.5f, 0.5f, 0.5f);

    glm::vec3 upRight(0, 0, 1);

    glm::vec3 color(0.5f, 0.5f, 0.5f);
    color = glm::vec3(0.5f, 0.25f, 0.f);
    if(position[2] > -0.05f){
        color = glm::vec3(0.f, 0.3f, 0.f);
    }

    float theta = std::acos(glm::dot(normal, upRight) / glm::length(normal) / glm::length(upRight));

    if(position[2] > 0.05f && std::abs(theta) < std::numbers::pi / 4){
        color = white;
    }
    // Return white as placeholder
    return color;
}

// Computes the intensity of Perlin noise at some point
float MountainGenerator::computePerlin(float x, float y) {
    // Task 1: get grid indices (as ints)
    int left = std::floor(x);
    int right = left + 1;
    int top = std::floor(y);
    int bottom = top + 1;
    // Task 2: compute offset vectors
    glm::vec2 center(x, y);

    //    glm::vec2 pTopLeft(left, top);
    //    glm::vec2 pTopRight(right, top);
    //    glm::vec2 pBottomLeft(left, bottom);
    //    glm::vec2 pBottomRight(right, bottom);

    glm::vec2 offTopLeft(x - left, y - top);
    glm::vec2 offTopRight(x - right, y - top);
    glm::vec2 offBottomLeft(x - left, y - bottom);
    glm::vec2 offBottomRight(x - right, y - bottom);

    //    offTopLeft = center - pTopLeft;
    //    offTopRight = center - pTopRight;
    //    offBottomLeft = center - pBottomLeft;
    //    offBottomRight = center - pBottomRight;

    glm::vec2 randTopLeft = sampleRandomVector(top, left);
    glm::vec2 randTopRight = sampleRandomVector(top, right);
    glm::vec2 randBottomLeft = sampleRandomVector(bottom, left);
    glm::vec2 randBottomRight = sampleRandomVector(bottom, right);

    // Task 3: compute the dot product between the grid point direction vectors and its offset vectors
    // float A = ... // dot product between top-left direction and its offset
    // float B = ... // dot product between top-right direction and its offset
    // float C = ... // dot product between bottom-right direction and its offset
    // float D = ... // dot product between bottom-left direction and its offset
    float A = glm::dot(offTopLeft, randTopLeft);
    float B = glm::dot(offTopRight, randTopRight);
    float C = glm::dot(offBottomLeft, randBottomLeft);
    float D = glm::dot(offBottomRight, randBottomRight);
    // Task 5: Debug this line to properly use your interpolation function to produce the correct value

    float dx = x - left;
    float dy = y - top;
    float G = interpolate(A, B, dx);
    float H = interpolate(C, D, dx);
    float R = interpolate(G, H, dy);


    return R;

    // Return 0 as a placeholder
    //    return 0;
}
