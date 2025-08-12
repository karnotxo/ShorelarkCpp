#ifndef CSHORELARK_SIMULATION_CONSTANTS_H
#define CSHORELARK_SIMULATION_CONSTANTS_H

#include <cstddef>

namespace cshorelark::simulation {

// Common constants used throughout the simulation package
namespace constants {

// Since we're using C++17 and <numbers> is C++20, define constants as constexpr
constexpr float k_pi = 3.14159265358979323846F;
constexpr float k_pi_over2 = k_pi / 2.0F;
constexpr float k_pi_over4 = k_pi / 4.0F;
constexpr float k_two_pi = k_pi * 2.0F;
constexpr float k_deg_to_rad = k_pi / 180.0F;

// Alignment constants for performance
constexpr std::size_t k_alignment_value = 64;

}  // namespace constants

}  // namespace cshorelark::simulation

#endif  // CSHORELARK_SIMULATION_CONSTANTS_H
