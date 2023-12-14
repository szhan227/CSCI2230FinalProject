#ifndef SPHEREGENERATOR_H
#define SPHEREGENERATOR_H

#include <vector>
#include <glm/glm.hpp>

class Sphere {
public:
    Sphere(int phiTesselations, int thetaTesselations);
    void generateSphereData(int phiTesselations, int thetaTesselations);
    const std::vector<float>& getSphereData() const;

private:
    static glm::vec4 sphericalToCartesian(float phi, float theta);
    static void pushVec3(glm::vec4 vec, std::vector<float>* data);

    std::vector<float> data;
};

#endif // SPHEREGENERATOR_H