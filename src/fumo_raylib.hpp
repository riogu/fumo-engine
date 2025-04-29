#pragma once
#include <cmath>
#include <iostream> // IWYU pragma: export
#include <libassert/assert.hpp>

#include "constants/direction_constants.hpp"
#include "fumo_serialize_macros.hpp"
#include "raylib.h"
#include "raymath.h"
#define PRINT_NO_NAME(x) std::cerr << libassert::highlight_stringify(x) << '\n';

#define PRINT(x) \
    std::cerr << #x << " ---> " << libassert::highlight_stringify(x) << '\n';

struct FumoVec2 {
    float x {0.0f}; // Vector x component
    float y {0.0f}; // Vector y component

    inline Vector2 to_raylib_vec2() const { return {x, y}; }
    SERIALIZE(x, y)
};

struct FumoColor {
    unsigned char r; // FumoColor red value
    unsigned char g; // FumoColor green value
    unsigned char b; // FumoColor blue value
    unsigned char a; // FumoColor alpha value

    inline Color to_raylib_color() const { return {r, g, b, a}; }
    SERIALIZE(r, g, b, a)
};

struct FumoRect {
    // NOTE: does not store positions most of the time
    // (x and y are not used very often)
    // commonly the position is on a separate Body{} component
    //
    // here because raylib is c and i cant add a damn template to the source code
    SERIALIZE(x, y, width, height)
    float x; // FumoRect top-left corner position x
    float y; // FumoRect top-left corner position y
    float width; // FumoRect width
    float height; // FumoRect height

    void draw(const FumoColor& color, const FumoVec2& position) const;
    void draw_outline(const FumoColor& color, const FumoVec2& position) const;

    // void resize(const FumoVec2& mouse_position, const FumoVec2& body_position);

    Rectangle to_raylib_rect() { return {x, y, width, height}; }
};

inline FumoColor to_fumo_color(Color c) { return {c.r, c.g, c.b, c.a}; }

void print_direction(DIRECTION direction);

void FumoDrawCircleV(FumoVec2 center, float radius, FumoColor color);
void FumoDrawLineV(FumoVec2 StartPos, FumoVec2 EndPos, FumoColor color);
void FumoDrawRectV(FumoVec2 position, FumoVec2 size, FumoColor color);
void FumoDrawRect(FumoRect rect, FumoColor color);
void FumoDrawLineEx(FumoVec2 StartPos,
                    FumoVec2 EndPos,
                    float thick,
                    FumoColor color);

DIRECTION opposite_direction(DIRECTION direction);
[[nodiscard]] FumoVec2 direction_to_vector(DIRECTION direction);
[[nodiscard]] DIRECTION vector_to_direction(const FumoVec2& vector);

inline float ease_quad_in(float t) { return t * t; }

inline float ease_quad_out(float t) { return -(t * (t - 2)); }

// Modeled after the overshooting cubic y = x^3-x*sin(x*pi)
inline float BackEaseIn(float p) { return p * p * p - p * sin(p * M_PI); }

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
inline float BackEaseOut(float p) {
    float f = (1 - p);
    return 1 - (f * f * f - f * sin(f * M_PI));
}

inline float ExponentialEaseIn(float p) {
    return (p == 0.0) ? p : pow(2, 10 * (p - 1));
}

// Modeled after the exponential function y = -2^(-10x) + 1
inline float ExponentialEaseOut(float p) {
    return (p == 1.0) ? p : 1 - pow(2, -10 * p);
}

inline float ExponentialEaseInOut(float p) {
    if (p == 0.0 || p == 1.0) return p;

    if (p < 0.5) {
        return 0.5 * pow(2, (20 * p) - 10);
    } else {
        return -0.5 * pow(2, (-20 * p) + 10) + 1;
    }
}

inline float ElasticEaseOut(float p) {
    return sin(-13 * M_PI_2 * (p + 1)) * pow(2, -10 * p) + 1;
}

inline float ElasticEaseInOut(float p) {
    if (p < 0.5) {
        return 0.5 * sin(13 * M_PI_2 * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
    } else {
        return 0.5
            * (sin(-13 * M_PI_2 * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1))
               + 2);
    }
}

inline float BackEaseInOut(float p) {
    if (p < 0.5) {
        float f = 2 * p;
        return 0.5 * (f * f * f - f * sin(f * M_PI));
    } else {
        float f = (1 - (2 * p - 1));
        return 0.5 * (1 - (f * f * f - f * sin(f * M_PI))) + 0.5;
    }
}

inline float BounceEaseOut(float p) {
    if (p < 4 / 11.0) {
        return (121 * p * p) / 16.0;
    } else if (p < 8 / 11.0) {
        return (363 / 40.0 * p * p) - (99 / 10.0 * p) + 17 / 5.0;
    } else if (p < 9 / 10.0) {
        return (4356 / 361.0 * p * p) - (35442 / 1805.0 * p) + 16061 / 1805.0;
    } else {
        return (54 / 5.0 * p * p) - (513 / 25.0 * p) + 268 / 25.0;
    }
}

inline float BounceEaseIn(float p) { return 1 - BounceEaseOut(1 - p); }

inline float BounceEaseInOut(float p) {
    if (p < 0.5) {
        return 0.5 * BounceEaseIn(p * 2);
    } else {
        return 0.5 * BounceEaseOut(p * 2 - 1) + 0.5;
    }
}

inline FumoColor FumoColorAlpha(FumoColor color, float alpha) {
    return to_fumo_color(ColorAlpha(color.to_raylib_color(), alpha));
}

inline FumoVec2 FumoVec2ScaleDivision(FumoVec2 v, float scale) {
    FumoVec2 result = {v.x / scale, v.y / scale};
    return result;
}

inline float FumoVec2DotProduct(FumoVec2 v1, FumoVec2 v2) {
    return Vector2DotProduct(v1.to_raylib_vec2(), v2.to_raylib_vec2());
}

inline float FumoVec2Distance(FumoVec2 v1, FumoVec2 v2) {
    return Vector2Distance(v1.to_raylib_vec2(), v2.to_raylib_vec2());
}

[[nodiscard]] inline FumoVec2
FumoVec2MoveTowards(FumoVec2 v, FumoVec2 target, float max_distance) {
    Vector2 ve = Vector2MoveTowards(v.to_raylib_vec2(),
                                    target.to_raylib_vec2(),
                                    max_distance);
    return {ve.x, ve.y};
}

[[nodiscard]] inline FumoVec2
FumoVec2SmoothMoveTowards(FumoVec2 v, FumoVec2 target, float smoothing_factor) {
    return FumoVec2MoveTowards(v,
                               target,
                               FumoVec2Distance(v, target) / smoothing_factor);
}

inline float FumoVec2DistanceSqr(FumoVec2 v1, FumoVec2 v2) {
    return Vector2DistanceSqr(v1.to_raylib_vec2(), v2.to_raylib_vec2());
}

inline FumoVec2 to_fumo_vec2(Vector2 v) { return {v.x, v.y}; }

inline FumoVec2 FumoVec2Add(FumoVec2 lhs, FumoVec2 rhs) {
    return to_fumo_vec2(Vector2Add(lhs.to_raylib_vec2(), rhs.to_raylib_vec2()));
}

inline float FumoVec2Angle(FumoVec2 lhs, FumoVec2 rhs) {
    return Vector2Angle(lhs.to_raylib_vec2(), rhs.to_raylib_vec2());
}

inline FumoVec2 FumoVec2Rotate(FumoVec2 v, float angle) {
    return to_fumo_vec2(Vector2Rotate(v.to_raylib_vec2(), angle));
}

inline FumoVec2 FumoVec2Negate(FumoVec2 rhs) {
    return to_fumo_vec2(Vector2Negate(rhs.to_raylib_vec2()));
}

inline FumoVec2 FumoVec2Normalize(FumoVec2 lhs) {
    return to_fumo_vec2(Vector2Normalize(lhs.to_raylib_vec2()));
}

inline FumoVec2 FumoVec2Subtract(FumoVec2 lhs, FumoVec2 rhs) {
    return to_fumo_vec2(
        Vector2Subtract(lhs.to_raylib_vec2(), rhs.to_raylib_vec2()));
}

inline float FumoVec2Length(FumoVec2 v) {
    return Vector2Length(v.to_raylib_vec2());
}

inline FumoVec2 FumoVec2Scale(FumoVec2 lhs, float rhs) {
    return to_fumo_vec2(Vector2Scale(lhs.to_raylib_vec2(), rhs));
}

inline FumoVec2 FumoVec2Divide(FumoVec2 lhs, FumoVec2 rhs) {
    return to_fumo_vec2(
        Vector2Divide(lhs.to_raylib_vec2(), rhs.to_raylib_vec2()));
}

inline FumoVec2 FumoVec2Multiply(FumoVec2 lhs, FumoVec2 rhs) {
    return to_fumo_vec2(
        Vector2Multiply(lhs.to_raylib_vec2(), rhs.to_raylib_vec2()));
}

inline FumoVec2 operator+(const FumoVec2& lhs, const FumoVec2& rhs) {
    return FumoVec2Add(lhs, rhs);
}

inline const FumoVec2& operator+=(FumoVec2& lhs, const FumoVec2& rhs) {
    lhs = FumoVec2Add(lhs, rhs);
    return lhs;
}

inline FumoVec2 operator-(const FumoVec2& lhs, const FumoVec2& rhs) {
    return FumoVec2Subtract(lhs, rhs);
}

inline const FumoVec2& operator-=(FumoVec2& lhs, const FumoVec2& rhs) {
    lhs = FumoVec2Subtract(lhs, rhs);
    return lhs;
}

inline FumoVec2 operator*(const FumoVec2& lhs, const float& rhs) {
    return FumoVec2Scale(lhs, rhs);
}

inline FumoVec2 operator*(const float& lhs, const FumoVec2& rhs) {
    return FumoVec2Scale(rhs, lhs);
}

inline const FumoVec2& operator*=(FumoVec2& lhs, const float& rhs) {
    lhs = FumoVec2Scale(lhs, rhs);
    return lhs;
}

inline FumoVec2 operator*(const FumoVec2& lhs, const FumoVec2& rhs) {
    return FumoVec2Multiply(lhs, rhs);
}

inline const FumoVec2& operator*=(FumoVec2& lhs, const FumoVec2& rhs) {
    lhs = FumoVec2Multiply(lhs, rhs);
    return lhs;
}

inline FumoVec2 operator/(const FumoVec2& lhs, const float& rhs) {
    return FumoVec2Scale(lhs, 1.0f / rhs);
}

inline const FumoVec2& operator/=(FumoVec2& lhs, const float& rhs) {
    lhs = FumoVec2Scale(lhs, 1.0f / rhs);
    return lhs;
}

inline FumoVec2 operator/(const FumoVec2& lhs, const FumoVec2& rhs) {
    return FumoVec2Divide(lhs, rhs);
}

inline const FumoVec2& operator/=(FumoVec2& lhs, const FumoVec2& rhs) {
    lhs = FumoVec2Divide(lhs, rhs);
    return lhs;
}

inline bool operator==(const FumoVec2& lhs, const FumoVec2& rhs) {
    return FloatEquals(lhs.x, rhs.x) && FloatEquals(lhs.y, rhs.y);
}

inline bool operator!=(const FumoVec2& lhs, const FumoVec2& rhs) {
    return !FloatEquals(lhs.x, rhs.x) || !FloatEquals(lhs.y, rhs.y);
}

[[nodiscard]] FumoVec2 FumoVec2Snap8Directions(FumoVec2 v1);
[[nodiscard]] FumoVec2 FumoVec2Snap4Directions(FumoVec2 v1);
// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define FUMO_LIGHTGRAY CLITERAL(FumoColor) {200, 200, 200, 255} // Light Gray
#define FUMO_GRAY CLITERAL(FumoColor) {130, 130, 130, 255} // Gray
#define FUMO_DARKGRAY CLITERAL(FumoColor) {80, 80, 80, 255} // Dark Gray
#define FUMO_YELLOW CLITERAL(FumoColor) {253, 249, 0, 255} // Yellow
#define FUMO_GOLD CLITERAL(FumoColor) {255, 203, 0, 255} // Gold
#define FUMO_ORANGE CLITERAL(FumoColor) {255, 161, 0, 255} // Orange
#define FUMO_PINK CLITERAL(FumoColor) {255, 109, 194, 255} // Pink
#define FUMO_RED CLITERAL(FumoColor) {230, 41, 55, 255} // Red
#define FUMO_MAROON CLITERAL(FumoColor) {190, 33, 55, 255} // Maroon
#define FUMO_GREEN CLITERAL(FumoColor) {0, 228, 48, 255} // Green
#define FUMO_LIME CLITERAL(FumoColor) {0, 158, 47, 255} // Lime
#define FUMO_DARKGREEN CLITERAL(FumoColor) {0, 117, 44, 255} // Dark Green
#define FUMO_SKYBLUE CLITERAL(FumoColor) {102, 191, 255, 255} // Sky Blue
#define FUMO_BLUE CLITERAL(FumoColor) {0, 121, 241, 255} // Blue
#define FUMO_DARKBLUE CLITERAL(FumoColor) {0, 82, 172, 255} // Dark Blue
#define FUMO_PURPLE CLITERAL(FumoColor) {200, 122, 255, 255} // Purple
#define FUMO_VIOLET CLITERAL(FumoColor) {135, 60, 190, 255} // Violet
#define FUMO_DARKPURPLE CLITERAL(FumoColor) {112, 31, 126, 255} // Dark Purple
#define FUMO_BEIGE CLITERAL(FumoColor) {211, 176, 131, 255} // Beige
#define FUMO_BROWN CLITERAL(FumoColor) {127, 106, 79, 255} // Brown
#define FUMO_DARKBROWN CLITERAL(FumoColor) {76, 63, 47, 255} // Dark Brown
#define FUMO_WHITE CLITERAL(FumoColor) {255, 255, 255, 255} // White
#define FUMO_BLACK CLITERAL(FumoColor) {0, 0, 0, 255} // Black
#define FUMO_BLANK CLITERAL(FumoColor) {0, 0, 0, 0} // Blank (Transparent)
#define FUMO_MAGENTA CLITERAL(FumoColor) {255, 0, 255, 255} // Magenta
#define FUMO_RAYWHITE CLITERAL(FumoColor) {245, 245, 245, 255}

constexpr static int screenWidth = 1920;
constexpr static int screenHeight = 1080;
constexpr static FumoVec2 screenCenter = {(float)screenWidth / 2.0f,
                                          (float)screenHeight / 2.0f};

constexpr FumoVec2 starter_player_position = {.x = 1920.0f / 2.0f,
                                              .y = screenCenter.y};

const float mouse_radius = 120.0f;
