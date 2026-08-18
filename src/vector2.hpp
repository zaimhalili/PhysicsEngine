#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <cmath>

struct Vector2D {
  float x;
  float y;
};

inline Vector2D Add(Vector2D a, Vector2D b) { return {a.x + b.x, a.y + b.y}; }

inline Vector2D Subtract(Vector2D a, Vector2D b) {
  return {a.x - b.x, a.y - b.y};
}

inline Vector2D Multiply(Vector2D v, float scalar) {
  return {v.x * scalar, v.y * scalar};
}

inline float LengthSqr(Vector2D v) { return (v.x * v.x) + (v.y * v.y); }

inline float Length(Vector2D v) { return std::sqrt(LengthSqr(v)); }

inline float Dot(Vector2D a, Vector2D b) {
  return (a.x * b.x) + (a.y * b.y);
}

#endif