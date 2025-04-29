#pragma once
#include <utility>
#include <vector>

#include "fumo_engine/components.hpp"
enum struct SHAPE { NO_SHAPE, Rectangle, Circle, Capsule };

// WARNING:
// not all collision information is guaranteed to be filled!
struct Collision {
    float overlap = 0.0f;
    // NOTE: if the collision is with a rectangle corner,
    // it wont be normal to one of the sides,
    // it will connect the collided corner with the shape at some angle
    FumoVec2 push_direction = {0.0f, 0.0f};
    FumoVec2 normal = {0.0f, 0.0f};
    DIRECTION collided_capsule_side = DIRECTION::NO_DIRECTION;
    // NOTE: there isnt an intersection point between 2 circles
    FumoVec2 intersection_point = {0.0f, 0.0f};
    float distance = 0.0f;
    bool collided = false;
    SHAPE collided_shape = SHAPE::NO_SHAPE;
    bool corner_collision = false;

    SERIALIZE(overlap,
              push_direction,
              normal,
              collided_capsule_side,
              intersection_point,
              distance,
              collided,
              collided_shape)
};

struct RectBodyPair {
    FumoRect fumo_rect;
    Body body;
};

namespace Collisions {

#define LINE_TO_SIDE_COLLISION(line, side, shape) \
    do { \
        Collision collision = LineToLineCollision(line, side); \
        collision.distance = \
            FumoVec2Distance(line.start, collision.intersection_point); \
        if (collision.collided \
            && (collision.distance < closest_collision.distance \
                || closest_collision.distance == 0.0f)) { \
            collision.push_direction = \
                FumoVec2Normalize(line.start - collision.intersection_point); \
            collision.overlap = \
                FumoVec2Distance(line.start, line.end) - collision.distance; \
            collision.collided_shape = shape; \
            collision.normal = \
                FumoVec2Snap4Directions(collision.push_direction); \
            closest_collision = collision; \
        } \
    } while (0)

void calculate_collided_region(Collision& collision,
                               const Capsule& player_capsule);
[[nodiscard]] Collision
continuous_rect_collision_solving(Capsule& player_shape,
                                  Body& player_body,
                                  const FumoRect& fumo_rect,
                                  const Body& fumo_rect_body,
                                  const int& substep_count);
[[nodiscard]] std::vector<RectBodyPair>
calculate_sub_rectangles(const FumoRect& fumo_rect,
                         const Body& fumo_rect_body,
                         const int& substep_count,
                         const int& max_sub_distance);

Collision capsule_to_rect_collision_solving(Capsule& player_shape,
                                            Body& player_body,
                                            const FumoRect& fumo_rect,
                                            const Body& fumo_rect_body);

Collision capsule_to_circle_collision_solving(Capsule& player_shape,
                                              Body& player_body,
                                              const Circle& circle_shape,
                                              const Body& circle_body,
                                              const int& substep_count);
[[nodiscard]] std::pair<float, FumoVec2>
PointToLineDistanceAndIntersection(const FumoVec2& Point,
                                   const FumoVec2& LineStart,
                                   const FumoVec2& LineEnd);
[[nodiscard]] Collision PointToLineCollision(const FumoVec2& Point,
                                             const Line& line);
[[nodiscard]] std::pair<float, FumoVec2> closest_fumo_rect_side(
    const std::vector<std::pair<float, FumoVec2>>& distances);

[[nodiscard]] FumoVec2 closest_point(const FumoVec2 target,
                                     const std::vector<FumoVec2>& points);

[[nodiscard]] bool LineToLineCollided(const Line& line1, const Line& line2);

[[nodiscard]] bool LineToRectCollided(const Line& line,
                                      const FumoRect& rect,
                                      const Body& rect_body);

[[nodiscard]] Collision LineToLineCollision(const Line& line1,
                                            const Line& line2);
[[nodiscard]] const Collision LineToRectCollision(const Line& line,
                                                  const FumoRect& rect,
                                                  const Body& rect_body);
[[nodiscard]] const Collision LineToCircleCollision(const Line& line,
                                                    const Circle& circle_shape,
                                                    const Body& circle_body);

[[nodiscard]] FumoVec2 ClosestCircleLineIntersection(const FumoVec2& Point,
                                                     const float& radius,
                                                     const Line& line);

[[nodiscard]] std::pair<float, FumoVec2>
CircleToRectDistanceAndIntersection(const FumoVec2& circle_center,
                                    const float& radius,
                                    const FumoRect& rect,
                                    const Body& rect_body);

[[nodiscard]] Collision CapsuleToRectCollision(const Capsule& player_shape,
                                               const Body& player_body,
                                               const FumoRect& fumo_rect,
                                               const Body& rectange_body);

[[nodiscard]] Collision CapsuleToCircleCollision(const Capsule& player_shape,
                                                 const Body& player_body,
                                                 const Circle& circle_shape,
                                                 const Body& circle_body);

[[nodiscard]] bool LineToCapsuleCollided(const Capsule& player_capsule,
                                         const Body& player_body,
                                         const Line& line,
                                         const Body& line_body);
[[nodiscard]] bool LineToCapsuleCollided(const Capsule& player_capsule,
                                         const Body& player_body,
                                         const Line& line);

[[nodiscard]] Collision LineToCapsuleCollision(const Capsule& player_capsule,
                                               const Body& player_body,
                                               const Line& line,
                                               const Body& line_body);
[[nodiscard]] Collision LineToCapsuleCollision(const Capsule& player_capsule,
                                               const Body& player_body,
                                               const Line& line);

} // namespace Collisions
