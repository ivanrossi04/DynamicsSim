#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

namespace Physics {
    // Physical constants
    static const float g = 9.806f; // gravitational acceleration in m/s^2
    static const float G = 6.67430e-11f; // gravitational constant for simulation
    static const float epsilon_0 = 8.854187817e-12f; // vacuum permittivity in F/m
    static const float k_e = 1.0f / (4.0f * M_PI * epsilon_0); // Coulomb's constant in N·m²/C²
    static const float mu_0 = 1.256637061e-6f; // vacuum permeability in H/m
    static const float k_m = mu_0 / (4.0f * M_PI); // Magnetic constant in N/A²

    /* 
    * Forces: base class and specific derived classes
    * The Force class defines the interface for computing forces and energies for different physical phenomena.
    * It is intended to represent the action of a force to particles in the space.
    * As such, it provides two methods:
    *
    * - computeForce: to calculate the force acting on a particle
    * - computeEnergy: to calculate the potential energy associated with a particle
    * 
    * Intended usage:
    * Every particle in the simulation should be associated with forces acting on it, that can be reused among different particles.
    * To represent multiple forces acting on a single particle, the CompositeForce class can be used
    */

    // Base class for forces
    class Force {
        public:
            virtual glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const = 0;
            virtual float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const = 0;
    };

    // Aggregation of multiple forces
    class CompositeForce : public Force {
        private:
            std::vector<const Force*> forces;
        public:
        
            void addForce(const Force& force) {
                forces.push_back(&force);
            }

            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                glm::vec3 totalForce(0.0f);
                
                for (const auto& force : forces) {
                    totalForce += force -> computeForce(position, velocity, time);
                }

                return totalForce;
            }

            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                float totalEnergy = 0.0f;

                for (const auto& force : forces) {
                    totalEnergy += force -> computeEnergy(position, velocity, time);
                }

                return totalEnergy;
            }
    };

    // Specific force implementations -----------------------------------------------------

    // Electric force between two charges
    class ElectricForce : public Force {
        private:
            float charge_1, charge_2;
            glm::vec3 anchorPoint;
        public:
            ElectricForce(float q1, float q2, const glm::vec3& anchor = glm::vec3(0.0f)) : charge_1(q1), charge_2(q2), anchorPoint(anchor) {}

            void setCharge1(float *q1) {charge_1 = *q1;}
            void setCharge2(float *q2) {charge_2 = *q2;}
            void setAnchorPoint(const glm::vec3& anchor) {anchorPoint = anchor;}

            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                return -k_e * charge_1 * charge_2 / (float) pow(glm::length(position - anchorPoint), 3) * (position - anchorPoint);
            }

            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                return -k_e * charge_1 * charge_2 / glm::length(position - anchorPoint);
            }
    };

    // Gravitational force between two masses
    class GravitationalForce : public Force {
        private:
            float mass_1, mass_2;
            glm::vec3 anchorPoint;
        public:
            GravitationalForce(float m1, float m2, const glm::vec3& anchor = glm::vec3(0.0f)) : mass_1(m1), mass_2(m2), anchorPoint(anchor) {}

            void setMass1(float *m1) {mass_1 = *m1;}
            void setMass2(float *m2) {mass_2 = *m2;}
            void setAnchorPoint(const glm::vec3& anchor) {anchorPoint = anchor;}

            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                return -G * mass_1 * mass_2 / (float) pow(glm::length(position - anchorPoint), 3) * (position - anchorPoint);
            }

            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                return -G * mass_1 * mass_2 / glm::length(position - anchorPoint);
            }
    };

    // Gravitational force near Earth's surface
    class EarthGravitationalForce : public Force {
        private:
            float mass;
        public:
            EarthGravitationalForce(float m) : mass(m) {}

            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                return glm::vec3(0.0f, -mass * g, 0.0f);
            }

            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                return mass * g * position.y;
            }
    };

    // Spring force using Hooke's law
    class HookeForce : public Force {
        private:
            glm::vec3 anchorPoint;
            float k; // spring constant
        public:
            HookeForce(float springConstant, const glm::vec3& anchor = glm::vec3(0.0f)) : anchorPoint(anchor), k(springConstant) {}

            void setAnchorPoint(const glm::vec3& anchor) {anchorPoint = anchor;}

            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                return -k * (position - anchorPoint);
            }

            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                return 0.5f * k * glm::dot(position - anchorPoint, position - anchorPoint);
            }
    };

    // Generic air resistance force (drag)
    class AirResistanceForce : public Force {
        private:
            float dragCoefficient;
        public:
            AirResistanceForce(float drag) : dragCoefficient(drag) {}

            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                return -dragCoefficient * velocity;
            }
            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override {
                return 0.5f * dragCoefficient * glm::dot(velocity, velocity);
            }
    };
}

namespace Propagation {
    class generalizedVector {
        public:
            glm::vec3 position;
            glm::vec3 velocity;

            generalizedVector() : position(glm::vec3(0.0f)), velocity(glm::vec3(0.0f)) {}
            generalizedVector(const glm::vec3& pos, const glm::vec3& vel) : position(pos), velocity(vel) {}
    };

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
}

#endif