#include "sphere.h"
#include "glm/gtc/constants.hpp"

Sphere::Sphere(int phiTesselations, int thetaTesselations) {
    generateSphereData(phiTesselations, thetaTesselations);
}

const std::vector<float>& Sphere::getSphereData() const {
    return data;
}

void Sphere::generateSphereData(int phiTesselations, int thetaTesselations) {
    data.clear();
    data.reserve(phiTesselations * thetaTesselations * 6 * 3);

    for(int iTheta = 0; iTheta < thetaTesselations; iTheta++) {
        for(int iPhi = 0; iPhi < phiTesselations; iPhi++) {
            float phi1 = 1.0  * iPhi / phiTesselations * glm::pi<float>();
            float phi2 = 1.0  * (iPhi + 1) / phiTesselations * glm::pi<float>();

            float the1 = 1.0 * iTheta / thetaTesselations * 2 * glm::pi<float>();
            float the2 = 1.0 * (iTheta + 1) / thetaTesselations * 2 * glm::pi<float>();

            glm::vec4 p1 = sphericalToCartesian(phi1,the1);
            glm::vec4 p2 = sphericalToCartesian(phi2,the1);
            glm::vec4 p3 = sphericalToCartesian(phi2,the2);
            glm::vec4 p4 = sphericalToCartesian(phi1,the2);

            pushVec3(p1,&data);
            pushVec3(p2,&data);
            pushVec3(p3,&data);

            pushVec3(p1,&data);
            pushVec3(p3,&data);
            pushVec3(p4,&data);
        }
    }
}

glm::vec4 Sphere::sphericalToCartesian(float phi, float theta) {
    return glm::vec4(glm::cos(theta) * glm::sin(phi),
                     glm::sin(theta) * glm::sin(phi),
                     glm::cos(phi),1);
}

void Sphere::pushVec3(glm::vec4 vec, std::vector<float>* data) {
    data->push_back(vec.x);
    data->push_back(vec.y);
    data->push_back(vec.z);
}
