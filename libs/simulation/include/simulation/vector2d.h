#ifndef CSHORELARK_SIMULATION_VECTOR2D_H
#define CSHORELARK_SIMULATION_VECTOR2D_H

// C++ system headers
#include <algorithm>
#include <cmath>

namespace cshorelark::simulation {

namespace defaults {
constexpr float k_coordinate_x = 0.0F;  // Default X coordinate
constexpr float k_coordinate_y = 0.0F;  // Default Y coordinate
}  // namespace defaults

/**
 * @brief A 2D vector class for representing positions and directions.
 *
 * @details This class provides basic 2D vector operations needed for the simulation,
 * including addition, subtraction, scalar multiplication, and normalization.
 *
 * Example usage:
 * @code
 * vector2d pos{10.0F, 20.0F};
 * vector2d dir{1.0F, 0.0F};
 * pos += dir * 5.0F;
 * dir.normalize();
 * @endcode
 */
class vector2d {
public:
    /**
     * @brief Default constructor. Creates a zero vector.
     */
    vector2d() = default;

    /**
     * @brief Constructs a vector with given x and y components.
     * @param coord_x The x coordinate
     * @param coord_y The y coordinate
     */
    vector2d(float coord_x, float coord_y) : x_value_(coord_x), y_value_(coord_y) {}

    /**
     * @brief Adds another vector to this one.
     * @param other Vector to add
     * @return Reference to this vector after addition
     */
    auto operator+=(const vector2d& other) noexcept -> vector2d& {
        x_value_ += other.x_value_;
        y_value_ += other.y_value_;
        return *this;
    }

    /**
     * @brief Subtracts another vector from this one.
     * @param other Vector to subtract
     * @return Reference to this vector after subtraction
     */
    auto operator-=(const vector2d& other) noexcept -> vector2d& {
        x_value_ -= other.x_value_;
        y_value_ -= other.y_value_;
        return *this;
    }

    /**
     * @brief Multiplies this vector by a scalar.
     * @param scalar The scalar value
     * @return Reference to this vector after multiplication
     */
    auto operator*=(float scalar) noexcept -> vector2d& {
        x_value_ *= scalar;
        y_value_ *= scalar;
        return *this;
    }

    /**
     * @brief Gets the length (magnitude) of the vector.
     * @return The vector's length
     */
    [[nodiscard]] auto length() const noexcept -> float { return hypotf(x_value_, y_value_); }

    /**
     * @brief Gets the squared length of the vector.
     * @return The vector's squared length
     */
    [[nodiscard]] auto length_squared() const noexcept -> float {
        return x_value_ * x_value_ + y_value_ * y_value_;
    }

    /**
     * @brief Normalizes this vector (makes it unit length).
     * @return Reference to this vector after normalization
     * @note If the vector has zero length, it remains unchanged
     */
    auto normalize() noexcept -> vector2d& {
        float const vec_length = length();
        if (vec_length > 0.0F) {
            x_value_ /= vec_length;
            y_value_ /= vec_length;
        }
        return *this;
    }

    /**
     * @brief Returns a normalized copy of this vector.
     * @return A new normalized vector
     */
    [[nodiscard]] auto normalized() const noexcept -> vector2d {
        vector2d result = *this;
        result.normalize();
        return result;
    }

    /**
     * @brief Computes the dot product with another vector.
     * @param other The other vector
     * @return The dot product
     */
    [[nodiscard]] auto dot(const vector2d& other) const noexcept -> float {
        return x_value_ * other.x_value_ + y_value_ * other.y_value_;
    }

    /**
     * @brief Computes the cross product with another vector.
     * @param other The other vector
     * @return The magnitude of the cross product (z component)
     * @note In 2D, cross product is a scalar (the z component of the 3D cross product)
     */
    [[nodiscard]] auto cross(const vector2d& other) const noexcept -> float {
        return x_value_ * other.y_value_ - y_value_ * other.x_value_;
    }

    /**
     * @brief Rotates the vector by a given angle.
     * @param angle_radians The angle in radians
     * @return Reference to this vector after rotation
     */
    auto rotate(float angle_radians) noexcept -> vector2d& {
        float const cos_angle = cosf(angle_radians);
        float const sin_angle = sinf(angle_radians);
        float const new_x = x_value_ * cos_angle - y_value_ * sin_angle;
        float const new_y = x_value_ * sin_angle + y_value_ * cos_angle;
        x_value_ = new_x;
        y_value_ = new_y;
        return *this;
    }

    /**
     * @brief Returns a rotated copy of this vector.
     * @param angle_radians The angle in radians
     * @return A new rotated vector
     */
    [[nodiscard]] auto rotated(float angle_radians) const noexcept -> vector2d {
        vector2d result = *this;
        result.rotate(angle_radians);
        return result;
    }

    /**
     * @brief Gets the angle between this vector and another vector.
     * @param other The other vector
     * @return The angle in radians
     */
    [[nodiscard]] auto angle_to(const vector2d& other) const noexcept -> float {
        const float dot_prod = dot(other);
        const float lengths_prod = length() * other.length();
        if (lengths_prod == 0.0F) {
            return 0.0F;
        }
        float cos_angle = dot_prod / lengths_prod;
        // Clamp to [-1, 1] to handle floating point errors
        cos_angle = std::clamp(-1.0F, 1.0F, cos_angle);
        return acosf(cos_angle);
    }

    /**
     * @brief Gets the angle of this vector relative to the positive x-axis.
     * @return The angle in radians in the range [-π, π]
     */
    [[nodiscard]] auto angle() const noexcept -> float { return atan2(y_value_, x_value_); }

    // Accessors
    [[nodiscard]] auto x() const noexcept -> float { return x_value_; }
    [[nodiscard]] auto y() const noexcept -> float { return y_value_; }

    // Mutators
    void set_x(float coord_x) noexcept { x_value_ = coord_x; }
    void set_y(float coord_y) noexcept { y_value_ = coord_y; }

private:
    float x_value_{0.0F};  ///< X coordinate
    float y_value_{0.0F};  ///< Y coordinate
};

/**
 * @brief Adds two vectors.
 * @param lhs Left-hand side vector
 * @param rhs Right-hand side vector
 * @return Result of addition
 */
[[nodiscard]] inline auto operator+(vector2d lhs, const vector2d& rhs) noexcept -> vector2d {
    return lhs += rhs;
}

/**
 * @brief Subtracts two vectors.
 * @param lhs Left-hand side vector
 * @param rhs Right-hand side vector
 * @return Result of subtraction
 */
[[nodiscard]] inline auto operator-(vector2d lhs, const vector2d& rhs) noexcept -> vector2d {
    return lhs -= rhs;
}

/**
 * @brief Multiplies a vector by a scalar.
 * @param vec The vector
 * @param scalar The scalar value
 * @return Result of multiplication
 */
[[nodiscard]] inline auto operator*(vector2d vec, float scalar) noexcept -> vector2d {
    return vec *= scalar;
}

/**
 * @brief Multiplies a scalar by a vector.
 * @param scalar The scalar value
 * @param vec The vector
 * @return Result of multiplication
 */
[[nodiscard]] inline auto operator*(float scalar, vector2d vec) noexcept -> vector2d {
    return vec *= scalar;
}

/**
 * @brief Computes the dot product of two vectors.
 * @param lhs Left-hand side vector
 * @param rhs Right-hand side vector
 * @return The dot product
 */
[[nodiscard]] inline auto dot(const vector2d& lhs, const vector2d& rhs) noexcept -> float {
    return lhs.dot(rhs);
}

/**
 * @brief Computes the cross product of two vectors.
 * @param lhs Left-hand side vector
 * @param rhs Right-hand side vector
 * @return The magnitude of the cross product (z component)
 */
[[nodiscard]] inline auto cross(const vector2d& lhs, const vector2d& rhs) noexcept -> float {
    return lhs.cross(rhs);
}

/**
 * @brief Equality comparison operator.
 * @param lhs Left-hand side vector
 * @param rhs Right-hand side vector
 * @return True if vectors are equal
 */
[[nodiscard]] inline auto operator==(const vector2d& lhs, const vector2d& rhs) noexcept -> bool {
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

/**
 * @brief Inequality comparison operator.
 * @param lhs Left-hand side vector
 * @param rhs Right-hand side vector
 * @return True if vectors are not equal
 */
[[nodiscard]] inline auto operator!=(const vector2d& lhs, const vector2d& rhs) noexcept -> bool {
    return !(lhs == rhs);
}

}  // namespace cshorelark::simulation

#endif  // CSHORELARK_SIMULATION_VECTOR2D_H_