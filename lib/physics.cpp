#include "physics.hpp"

namespace Physics {

    // CompositeForce implementations
    void CompositeForce::addForce(const Force& force) {
        forces.push_back(&force);
    }

    void CompositeForce::removeForce(const Force& force) {
        for (std::vector<const Force*>::iterator it = forces.begin(); it != forces.end(); ++it) {
            if (*it == &force) {
                forces.erase(it);
                break;
            }
        }
    }

    glm::vec3 CompositeForce::computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        glm::vec3 totalForce(0.0f);
        
        for (const auto& force : forces) {
            totalForce += force->computeForce(position, velocity, time);
        }

        return totalForce;
    }

    float CompositeForce::computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        float totalEnergy = 0.0f;

        for (const auto& force : forces) {
            totalEnergy += force->computeEnergy(position, velocity, time);
        }

        return totalEnergy;
    }

    // ElectricForce implementations
    ElectricForce::ElectricForce(float q1, float q2, const glm::vec3& anchor) 
        : charge_1(q1), charge_2(q2), anchorPoint(anchor) {}

    float ElectricForce::getCharge1() const {
        return charge_1;
    }

    float ElectricForce::getCharge2() const {
        return charge_2;
    }

    void ElectricForce::setCharge1(float q1) {
        charge_1 = q1;
    }

    void ElectricForce::setCharge2(float q2) {
        charge_2 = q2;
    }

    void ElectricForce::setAnchorPoint(const glm::vec3& anchor) {
        anchorPoint = anchor;
    }

    glm::vec3 ElectricForce::computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        return -k_e * charge_1 * charge_2 / (float)pow(glm::length(position - anchorPoint), 3) * (position - anchorPoint);
    }

    float ElectricForce::computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        return -k_e * charge_1 * charge_2 / glm::length(position - anchorPoint);
    }

    // GravitationalForce implementations
    GravitationalForce::GravitationalForce(float m1, float m2, const glm::vec3& anchor) 
        : mass_1(m1), mass_2(m2), anchorPoint(anchor) {}

    float GravitationalForce::getMass1() const {
        return mass_1;
    }

    float GravitationalForce::getMass2() const {
        return mass_2;
    }

    void GravitationalForce::setMass1(float m1) {
        mass_1 = m1;
    }

    void GravitationalForce::setMass2(float m2) {
        mass_2 = m2;
    }

    void GravitationalForce::setAnchorPoint(const glm::vec3& anchor) {
        anchorPoint = anchor;
    }

    glm::vec3 GravitationalForce::computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        return -G * mass_1 * mass_2 / (float)pow(glm::length(position - anchorPoint), 3) * (position - anchorPoint);
    }

    float GravitationalForce::computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        return -G * mass_1 * mass_2 / glm::length(position - anchorPoint);
    }

    // EarthGravitationalForce implementations
    EarthGravitationalForce::EarthGravitationalForce(float m) : mass(m) {}

    glm::vec3 EarthGravitationalForce::computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        return glm::vec3(0.0f, -mass * g, 0.0f);
    }

    float EarthGravitationalForce::computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        return mass * g * position.y;
    }

    // HookeForce implementations
    HookeForce::HookeForce(float springConstant, const glm::vec3& anchor) 
        : anchorPoint(anchor), k(springConstant) {}

    void HookeForce::setAnchorPoint(const glm::vec3& anchor) {
        anchorPoint = anchor;
    }

    glm::vec3 HookeForce::computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        return -k * (position - anchorPoint);
    }

    float HookeForce::computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        return 0.5f * k * glm::dot(position - anchorPoint, position - anchorPoint);
    }

    // AirResistanceForce implementations
    AirResistanceForce::AirResistanceForce(float drag) : dragCoefficient(drag) {}

    glm::vec3 AirResistanceForce::computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        return -dragCoefficient * velocity;
    }

    float AirResistanceForce::computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const {
        return 0.5f * dragCoefficient * glm::dot(velocity, velocity);
    }
}

namespace Propagation {

    // generalizedVector implementations
    generalizedVector::generalizedVector() 
        : position(glm::vec3(0.0f)), velocity(glm::vec3(0.0f)) {}

    generalizedVector::generalizedVector(const glm::vec3& pos, const glm::vec3& vel) 
        : position(pos), velocity(vel) {}

    // Propagation methods

    generalizedVector explicitEuler(Physics::Force* f, const generalizedVector& state, const float mass, const float currentTime, const float deltaTime) {
        glm::vec3 force = f->computeForce(state.position, state.velocity, currentTime);

        return {
            state.position + state.velocity * deltaTime,
            state.velocity + (force / mass) * deltaTime
        };
    }

    generalizedVector rungeKutta4(Physics::Force* f, const generalizedVector& state, const float mass, const float currentTime, const float deltaTime) {
        static glm::vec3 kx[4], kv[4];

        kx[0] = state.velocity;
        static glm::vec3 force = f->computeForce(state.position, kx[0], currentTime);
        kv[0] = force / mass;

        kx[1] = state.velocity + kv[0] * (deltaTime / 2.0f);
        force = f->computeForce(state.position + kx[0] * (deltaTime / 2.0f), kx[1], currentTime + deltaTime / 2.0f);
        kv[1] = force / mass;

        kx[2] = state.velocity + kv[1] * (deltaTime / 2.0f);
        force = f->computeForce(state.position + kx[1] * (deltaTime / 2.0f), kx[2], currentTime + deltaTime / 2.0f);
        kv[2] = force / mass;

        kx[3] = state.velocity + kv[2] * deltaTime;
        force = f->computeForce(state.position + kx[2] * deltaTime, kx[3], currentTime + deltaTime);
        kv[3] = force / mass;

        return {
            state.position + deltaTime / 6.0f * (kx[0] + 2.0f * kx[1] + 2.0f * kx[2] + kx[3]),
            state.velocity + deltaTime / 6.0f * (kv[0] + 2.0f * kv[1] + 2.0f * kv[2] + kv[3])
        };
    }

    generalizedVector simplecticEuler(Physics::Force* f, const generalizedVector& state, const float mass, const float currentTime, const float deltaTime) {
        glm::vec3 force = f->computeForce(state.position, state.velocity, currentTime);
        glm::vec3 newVelocity = state.velocity + (force / mass) * deltaTime;

        return {
            state.position + newVelocity * deltaTime,
            newVelocity
        };
    }

}

