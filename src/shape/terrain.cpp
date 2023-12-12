#include "terrain.h"

#include <cmath>
#include "glm/glm.hpp"

// Constructor
TerrainGenerator::TerrainGenerator()
{
  // Task 8: turn off wireframe shading
//   m_wireshade = true; // STENCIL CODE
  m_wireshade = false; // TA SOLUTION

  // Define resolution of terrain generation
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
TerrainGenerator::~TerrainGenerator()
{
    m_randVecLookup.clear();
}

// Helper for generateTerrain()
void addPointToVector_terrain(glm::vec3 point, std::vector<float>& vector) {
    vector.push_back(point.x);
    vector.push_back(point.z);
    vector.push_back(-point.y);
}

void addColorToVector_terrain(glm::vec3 color, std::vector<float>& vector){
    vector.push_back(color.x);
    vector.push_back(color.y);
    vector.push_back(color.z);
}

// Generates the geometry of the output triangle mesh
std::vector<float> TerrainGenerator::generateTerrain() {
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
            addPointToVector_terrain(p1, verts);
            addPointToVector_terrain(n1, verts);
            addColorToVector_terrain(getColor(n1, p1), verts);

            addPointToVector_terrain(p2, verts);
            addPointToVector_terrain(n2, verts);
            addColorToVector_terrain(getColor(n2, p2), verts);

            addPointToVector_terrain(p3, verts);
            addPointToVector_terrain(n3, verts);
            addColorToVector_terrain(getColor(n3, p3), verts);

            // tris 2
            // x1y1z1
            // x2y2z3
            // x1y2z4
            addPointToVector_terrain(p1, verts);
            addPointToVector_terrain(n1, verts);
            addColorToVector_terrain(getColor(n1, p1), verts);

            addPointToVector_terrain(p3, verts);
            addPointToVector_terrain(n3, verts);
            addColorToVector_terrain(getColor(n3, p3), verts);

            addPointToVector_terrain(p4, verts);
            addPointToVector_terrain(n4, verts);
            addColorToVector_terrain(getColor(n4, p4), verts);
        }
    }
    return verts;
}

// Samples the (infinite) random vector grid at (row, col)
glm::vec2 TerrainGenerator::sampleRandomVector(int row, int col)
{
    std::hash<int> intHash;
    int index = intHash(row * 41 + col * 43) % m_lookupSize;
    return m_randVecLookup.at(index);
}

// Takes a grid coordinate (row, col), [0, m_resolution), which describes a vertex in a plane mesh
// Returns a normalized position (x, y, z); x and y in range from [0, 1), and z is obtained from getHeight()
glm::vec3 TerrainGenerator::getPosition(int row, int col) {
    // Normalizing the planar coordinates to a unit square 
    // makes scaling independent of sampling resolution.
    int scale = 20;
    float center = scale / 2.f;
    float x = 1.f * scale * row / m_resolution;
    float y = 1.f * scale * col / m_resolution;

    float z;
    z = getHeight(x, y);
    float distance = std::sqrt((x - center) * (x - center) + (y - center) * (y - center));
    if(distance <= 1.5f){
        z *= 5 * (1.5 - distance);
    }
    else if(distance <= 3.f){
        float coef = (distance * distance / 9.f);
        coef = distance / 3.f;
        z *= coef;
        z -= 0.15;
    }
    return glm::vec3(x - 0.5f * scale,y - 0.5f * scale,z);
}

// ================== Students, please focus on the code below this point

// Helper for computePerlin() and, possibly, getColor()
float terrain_interpolate(float A, float B, float alpha) {
    // Task 4: implement your easing/interpolation function below
    float eased_a = 3 * pow(alpha, 2) - 2 * pow(alpha, 3);

    // Return 0 as placeholder
    return A + (B - A) * eased_a;
}

// Takes a normalized (x, y) position, in range [0,1)
// Returns a height value, z, by sampling a noise function
float TerrainGenerator::getHeight(float x, float y) {

    // Task 6: modify this call to produce noise of a different frequency
    float z = computePerlin(x * 5, y * 5) / 2;
    float temp_z = 0;
    float temp_x;
    float temp_y;

    // Task 7: combine multiple different octaves of noise to produce fractal perlin noise
    for (int freq_power = 0; freq_power < 4; freq_power++) {
        temp_x = x * pow(2, freq_power);
        temp_y = y * pow(2, freq_power);
        temp_z += computePerlin(temp_x * 5, temp_y * 5) / 2 / pow(2, freq_power+1);
    }

    // Return 0 as placeholder
    return temp_z + 0.15;
}

// Computes the normal of a vertex by averaging neighbors
glm::vec3 TerrainGenerator::getNormal(int row, int col) {
    // Task 9: Compute the average normal for the given input indices
    glm::vec3 normal = glm::vec3(0, 0, 0);
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
    glm::vec3 V = getPosition(row,col);
    for (int i = 0; i < 8; ++i) {
     int n1RowOffset = neighborOffsets[i][0];
     int n1ColOffset = neighborOffsets[i][1];
     int n2RowOffset = neighborOffsets[(i + 1) % 8][0];
     int n2ColOffset = neighborOffsets[(i + 1) % 8][1];
     glm::vec3 n1 = getPosition(row + n1RowOffset, col + n1ColOffset);
     glm::vec3 n2 = getPosition(row + n2RowOffset, col + n2ColOffset);
     normal = normal + glm::cross(n1 - V, n2 - V);
    }
    return glm::normalize(normal);
}

// Computes color of vertex using normal and, optionally, position
glm::vec3 TerrainGenerator::getColor(glm::vec3 normal, glm::vec3 position) {
    // Task 10: compute color as a function of the normal and position
    if (position[2] > 0.03 || glm::dot(normal, glm::vec3(0, 0, 1)) > 0.99) {
        return glm::vec3(1, 1, 1);
    } else {
        return glm::vec3(0.5, 0.5, 0.5);
    }
}

// Computes the intensity of Perlin noise at some point
float TerrainGenerator::computePerlin(float x, float y) {
    // Task 1: get grid indices (as ints)
    int x_small = (int) x;
    int x_large = x_small + 1;
    int y_small = (int) y;
    int y_large = y_small + 1;
    

    // Task 2: compute offset vectors
    glm::vec2 offset_top_left = glm::vec2(x - x_small, y - y_small);
    glm::vec2 offset_top_right = glm::vec2(x - x_large, y - y_small);
    glm::vec2 offset_bottom_right = glm::vec2(x - x_large, y - y_large);
    glm::vec2 offset_bottom_left = glm::vec2(x - x_small, y - y_large);

    // Task 3: compute the dot product between the grid point direction vectors and its offset vectors
    float A = glm::dot(offset_top_left, sampleRandomVector(x_small, y_small)); // dot product between top-left direction and its offset
    float B = glm::dot(offset_top_right, sampleRandomVector(x_large, y_small)); // dot product between top-right direction and its offset
    float C = glm::dot(offset_bottom_right, sampleRandomVector(x_large, y_large)); // dot product between bottom-right direction and its offset
    float D = glm::dot(offset_bottom_left, sampleRandomVector(x_small, y_large)); // dot product between bottom-left direction and its offset

    // Task 5: Debug this line to properly use your interpolation function to produce the correct value
    return terrain_interpolate(terrain_interpolate(A, B, x - x_small), terrain_interpolate(D, C, x - x_small), y - y_small);

    // Return 0 as a placeholder
    return 0;
}
