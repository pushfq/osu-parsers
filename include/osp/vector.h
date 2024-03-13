#pragma once

#include <cmath>

namespace osp {

template <typename T>
struct vector2 {
  T x, y;

  /**
   * @brief Default constructor.
   */
  constexpr vector2() : x{}, y{} {}

  /**
   * @brief Construct a vector from two values.
   * @param x X component.
   * @param y Y component.
   */
  constexpr vector2(T x, T y) : x(x), y(y) {}

  /**
   * @brief Construct a vector from a scalar value.
   * @param scalar Scalar value.
   */
  constexpr vector2(T scalar) : x(scalar), y(scalar) {}

#define VECTOR_OP(op, inplace_op)                         \
  constexpr vector2 operator op(vector2 other) const {    \
    return vector2(x op other.x, y op other.y);           \
  }                                                       \
                                                          \
  constexpr vector2 operator op(T scalar) const {         \
    return vector2(x op scalar, y op scalar);             \
  }                                                       \
                                                          \
  constexpr vector2& operator inplace_op(vector2 other) { \
    x inplace_op other.x;                                 \
    y inplace_op other.y;                                 \
    return *this;                                         \
  }                                                       \
                                                          \
  constexpr vector2& operator inplace_op(T scalar) {      \
    x inplace_op scalar;                                  \
    y inplace_op scalar;                                  \
    return *this;                                         \
  }

  VECTOR_OP(+, +=)
  VECTOR_OP(-, -=)
  VECTOR_OP(*, *=)
  VECTOR_OP(/, /=)

#undef VECTOR_OP

  /**
   * @brief Calculate the vector's length squared.
   * @return x^2 + y^2
   */
  constexpr T length_squared() const {
    return x * x + y * y;
  }

  /**
   * @brief Calculate the vector's length.
   * @return sqrt(x^2 + y^2)
   */
  constexpr T length() const {
    return std::sqrt(x * x + y * y);
  }

  /**
   * @brief Normalize the vector.
   * @return Normalized vector.
   */
  constexpr vector2 normalize() const {
    return *this / length();
  }
};

using vector2f = vector2<float>;

}  // namespace osp
