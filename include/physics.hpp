#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>
#include <glm/glm.hpp>

namespace Physics {
    // Physical constants
    static const float g = 9.806f; // Earth gravitational acceleration in m/s^2
    static const float G = 6.67430e-11f; // Gravitational constant in m^3/kg/s^2
    static const float epsilon_0 = 8.854187817e-12f; // Vacuum permittivity in F/m
    static const float k_e = 1.0f / (4.0f * M_PI * epsilon_0); // Coulomb's constant in N·m²/C²
    static const float mu_0 = 1.256637061e-6f; // Vacuum permeability in H/m
    static const float k_m = mu_0 / (4.0f * M_PI); // Magnetic constant in N/A²

    /**
     * @class Force
     * @brief Base class for different types of forces
     * 
     * 
     * Forces: base class and specific derived classes
     * The Force class defines the interface for computing forces and energies for different physical phenomena.
     * It is intended to represent the action of a force to particles in the space.
     * As such, it provides two methods:
     *
     * - computeForce: to calculate the force acting on a particle
     * 
     * - computeEnergy: to calculate the potential energy associated with a particle
     * 
     * Intended usage:
     * Every particle in the simulation should be associated with forces acting on it, that can be reused among different particles.
     * To represent multiple forces acting on a single particle, use the CompositeForce class.
     * 
    */
    class Force {
        public:
            virtual ~Force() = default;

            /// @brief Compute the force acting on a particle
            virtual glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const = 0;

            /// @brief Compute the potential energy associated with a particle
            virtual float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const = 0;
    };

    /// @brief Aggregation of multiple forces
    class CompositeForce : public Force {
        private:
            std::vector<const Force*> forces;
        public:
        
            /// @brief Add a new force to the composite force
            void addForce(const Force& force);

            /// @brief Remove a force from the composite force
            void removeForce(const Force& force);

            /// @brief Compute the total force acting on a particle by summing all individual forces
            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override;

            /// @brief Compute the total potential energy associated with a particle by summing all individual energies
            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override;
    };

    // Specific force declaration -----------------------------------------------------

    /// @brief Electric force between two charges
    class ElectricForce : public Force {
        private:
            float charge_1, charge_2;
            glm::vec3 anchorPoint;
        public:
            ElectricForce(float q1, float q2, const glm::vec3& anchor = glm::vec3(0.0f));

            void getCharge1(float *q1);
            void getCharge2(float *q2);

            void setCharge1(float *q1);
            void setCharge2(float *q2);

            /// @brief Set the anchor point for the force, which represents the position of the second charge
            void setAnchorPoint(const glm::vec3& anchor);

            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override;
            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override;
    };

    /// @brief Gravitational force between two masses
    class GravitationalForce : public Force {
        private:
            float mass_1, mass_2;
            glm::vec3 anchorPoint;
        public:
            GravitationalForce(float m1, float m2, const glm::vec3& anchor = glm::vec3(0.0f));

            void setMass1(float *m1);
            void setMass2(float *m2);
            void setAnchorPoint(const glm::vec3& anchor);

            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override;
            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override;
    };

    /// @brief Gravitational force near Earth's surface
    class EarthGravitationalForce : public Force {
        private:
            float mass;
        public:
            EarthGravitationalForce(float m);

            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override;
            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override;
    };

    /// @brief Spring force using Hooke's law
    class HookeForce : public Force {
        private:
            glm::vec3 anchorPoint;
            float k; // spring constant
        public:
            HookeForce(float springConstant, const glm::vec3& anchor = glm::vec3(0.0f));

            void setAnchorPoint(const glm::vec3& anchor);

            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override;
            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override;
    };

    /// @brief Generic air resistance force (drag)
    class AirResistanceForce : public Force {
        private:
            float dragCoefficient;
        public:
            AirResistanceForce(float drag);

            glm::vec3 computeForce(const glm::vec3& position, const glm::vec3& velocity, float time) const override;
            float computeEnergy(const glm::vec3& position, const glm::vec3& velocity, float time) const override;
    };
}

namespace Propagation {
    class generalizedVector {
        public:
            glm::vec3 position;
            glm::vec3 velocity;

            generalizedVector();
            generalizedVector(const glm::vec3& pos, const glm::vec3& vel);
    };

    generalizedVector explicitEuler(Physics::Force* f, const generalizedVector& state, const float mass, const float currentTime, const float deltaTime);
    generalizedVector rungeKutta4(Physics::Force* f, const generalizedVector& state, const float mass, const float currentTime, const float deltaTime);
}

#endif