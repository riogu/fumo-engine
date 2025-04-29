#include "fumo_engine/collisions_and_physics/collision_functions.hpp"

namespace Collisions {

[[nodiscard]] std::pair<float, FumoVec2> closest_fumo_rect_side(
    const std::vector<std::pair<float, FumoVec2>>& distances) {
    // used for picking one of 2 fumo_rect sides for solving a collision
    // with a circle
    // we pick the closest side to the circle center

    std::pair<float, FumoVec2> min_pair {6969696, {0.0f, 0.0f}};

    for (const auto& pair : distances) {
        const auto& dist = pair.first;
        if (dist < min_pair.first && dist != 0) {
            min_pair = pair;
        }
    }
    if (min_pair.first == 6969696) {
        return {0.0f, {0.0f, 0.0f}};
    }
    return min_pair;
}

[[nodiscard]] std::pair<float, FumoVec2>
CircleToRectDistanceAndIntersection(const FumoVec2& circle_center,
                                    const float& radius,
                                    const FumoRect& rect,
                                    const Body& rect_body) {

    FumoVec2 TopLeft = {rect_body.position.x, rect_body.position.y};
    FumoVec2 TopRight = {rect_body.position.x + rect.width,
                         rect_body.position.y};
    FumoVec2 BottomLeft = {rect_body.position.x,
                           rect_body.position.y + rect.height};
    FumoVec2 BottomRight = {rect_body.position.x + rect.width,
                            rect_body.position.y + rect.height};
    std::vector<FumoVec2> points {TopLeft, TopRight, BottomLeft, BottomRight};

    float intersection_distance;
    FumoVec2 closest_corner;
    std::pair<float, FumoVec2> closest_pair = {0.0f, {0.0f, 0.0f}};

    closest_corner = closest_point(circle_center, points);
    intersection_distance = FumoVec2Distance(circle_center, closest_corner);

    if (intersection_distance < radius) {
        // we have a collision with a corner
        // set our corner so it becomes the collision if no side matches
        closest_pair.first = intersection_distance;
        closest_pair.second = closest_corner;
    }
    // -------------------------------------------------------------------------------
    // we have to check the closest sides for collision if none of the corners
    // were a matching collision
    // -------------------------------------------------------------------------------

    std::pair<float, FumoVec2> left_pair, bottom_pair, right_pair, top_pair;

    bottom_pair = PointToLineDistanceAndIntersection(circle_center,
                                                     BottomLeft,
                                                     BottomRight);
    left_pair =
        PointToLineDistanceAndIntersection(circle_center, TopLeft, BottomLeft);
    right_pair = PointToLineDistanceAndIntersection(circle_center,
                                                    TopRight,
                                                    BottomRight);
    top_pair =
        PointToLineDistanceAndIntersection(circle_center, TopLeft, TopRight);

    // -------------------------------------------------------------------------------
    // weird_debug_print(closest_pair, left_pair, bottom_pair, right_pair, top_pair);

    if (closest_corner == TopLeft) {
        left_pair = PointToLineDistanceAndIntersection(circle_center,
                                                       TopLeft,
                                                       BottomLeft);
        top_pair = PointToLineDistanceAndIntersection(circle_center,
                                                      TopLeft,
                                                      TopRight);
        closest_pair = closest_fumo_rect_side({left_pair, top_pair});
        return closest_pair;
    }
    if (closest_corner == TopRight) {
        right_pair = PointToLineDistanceAndIntersection(circle_center,
                                                        TopRight,
                                                        BottomRight);
        top_pair = PointToLineDistanceAndIntersection(circle_center,
                                                      TopLeft,
                                                      TopRight);
        closest_pair = closest_fumo_rect_side({right_pair, top_pair});
        return closest_pair;
    }
    if (closest_corner == BottomLeft) {
        bottom_pair = PointToLineDistanceAndIntersection(circle_center,
                                                         BottomLeft,
                                                         BottomRight);
        left_pair = PointToLineDistanceAndIntersection(circle_center,
                                                       TopLeft,
                                                       BottomLeft);
        closest_pair = closest_fumo_rect_side({bottom_pair, left_pair});
        return closest_pair;
    }
    if (closest_corner == BottomRight) {
        right_pair = PointToLineDistanceAndIntersection(circle_center,
                                                        TopRight,
                                                        BottomRight);
        bottom_pair = PointToLineDistanceAndIntersection(circle_center,
                                                         BottomLeft,
                                                         BottomRight);
        closest_pair = closest_fumo_rect_side({right_pair, bottom_pair});
        return closest_pair;
    }
    return closest_pair;
}

// -------------------------------------------------------------------------------
// Explanation:
// -> Checking agaisnt the capsule's circles
// 1- we take our circle and find the distance between the center of the circle
//  and the closest point to the circle's center,
//  on any of the sides of the fumo_rect.
//  then we take the closest side and pick that as our collision side
//  and move the circle away from that point of intersection
//
// 2- if neither side is valid that means we are colliding with the corner.
//  this means we want to move the circle away from the corner, so we take
//  the closest corner and solve for that
// -------------------------------------------------------------------------------
#define CHECK_CAPSULE_CIRCLE(CIRCLE_CENTER) \
    do { \
        closest_dist_intersection = \
            CircleToRectDistanceAndIntersection(CIRCLE_CENTER, \
                                                player_capsule.radius, \
                                                fumo_rect, \
                                                rect_body); \
        if (closest_dist_intersection.first != 0.0f) { \
            if (closest_dist_intersection.first < player_capsule.radius) { \
                collision.overlap = \
                    player_capsule.radius - closest_dist_intersection.first; \
                collision.push_direction = FumoVec2Normalize( \
                    CIRCLE_CENTER - closest_dist_intersection.second); \
                collision.intersection_point = \
                    closest_dist_intersection.second; \
                collision.collided = true; \
                calculate_collided_region(collision, player_capsule); \
                return collision; \
            } \
        } \
    } while (0)

[[nodiscard]] Collision CapsuleToRectCollision(const Capsule& player_capsule,
                                               const Body& player_body,
                                               const FumoRect& fumo_rect,
                                               const Body& rect_body) {

    std::pair<float, FumoVec2> closest_dist_intersection = {0.0f, {0.0f, 0.0f}};
    Collision collision {};

    // -------------------------------------------------------------------------------
    // try the bottom circle for collisions
    CHECK_CAPSULE_CIRCLE(player_capsule.bottom_circle_center);
    // // try the top circle for collisions
    CHECK_CAPSULE_CIRCLE(player_capsule.top_circle_center);
    // -------------------------------------------------------------------------------

    return collision;

    // if (LineToRectCollided(player_capsule.left_line, fumo_rect, rect_body)) {
    //     // PRINT_NO_NAME("COLLISION HAPPENED LEFT")
    // }
    // if (LineToRectCollided(player_capsule.right_line, fumo_rect, rect_body)) {
    //
    //     // PRINT_NO_NAME("COLLISION HAPPENED RIGHT")
    // }
}

[[nodiscard]] Collision CapsuleToCircleCollision(const Capsule& player_capsule,
                                                 const Body& player_body,
                                                 const Circle& circle_shape,
                                                 const Body& circle_body) {

    // TODO: this is older code, eventually you should replace all
    // instances of std::pair<float, FumoVec2> (used for dist_intersection)
    // with just filling in fields of the Collision struct and returning it
    // (theres one function that uses this std::pair for other purposes though)

    float radius_sum = player_capsule.radius + circle_shape.radius;
    float overlap = 0.0f;
    FumoVec2 push_direction {0.0f, 0.0f};

    float correction = 0.5f;
    // -------------------------------------------------------------------------------
    // bottom circle collision check
    float bottom_distance =
        FumoVec2Distance(circle_body.position,
                         player_capsule.bottom_circle_center);
    if (bottom_distance < radius_sum + correction) {
        // collided with the bottom circle
        overlap = radius_sum - bottom_distance;
        push_direction = FumoVec2Normalize(player_capsule.bottom_circle_center
                                           - circle_body.position);
        Collision collision = {.overlap = overlap,
                               .push_direction = push_direction,
                               .intersection_point =
                                   player_capsule.bottom_circle_center
                                   - push_direction * player_capsule.radius,
                               .distance = bottom_distance,
                               .collided = true};
        Collisions::calculate_collided_region(collision, player_capsule);
        return collision;
    }
    // -------------------------------------------------------------------------------
    // top circle collision check
    float top_distance = FumoVec2Distance(circle_body.position,
                                          player_capsule.top_circle_center);

    if (top_distance < radius_sum) {
        // collided with top circle
        overlap = radius_sum - top_distance;
        push_direction = FumoVec2Normalize(player_capsule.top_circle_center
                                           - circle_body.position);

        Collision collision {.overlap = overlap,
                             .push_direction = push_direction,
                             .intersection_point =
                                 player_capsule.top_circle_center
                                 - push_direction * player_capsule.radius,
                             .distance = top_distance,
                             .collided = true};

        Collisions::calculate_collided_region(collision, player_capsule);

        return collision;
    }
    // -------------------------------------------------------------------------------
    // NOTE: remove the capsule sides later but for now i want to test them
    //
    // -------------------------------------------------------------------------------
    // capsule sides collision check
    const auto left_line_distance_pair =
        PointToLineDistanceAndIntersection(circle_body.position,
                                           player_capsule.left_line.start,
                                           player_capsule.left_line.end);

    if (left_line_distance_pair.first != 0
        && left_line_distance_pair.first < circle_shape.radius) {
        // collided with left_line
        overlap = circle_shape.radius - left_line_distance_pair.first;
        push_direction = FumoVec2Normalize(left_line_distance_pair.second
                                           - circle_body.position);
        Collision collision {.overlap = overlap,
                             .push_direction = push_direction,
                             .intersection_point =
                                 left_line_distance_pair.second,
                             .distance = left_line_distance_pair.first,
                             .collided = true};

        // Collisions::calculate_collided_side(collision, player_capsule);

        return collision;
    }

    const auto right_line_distance_pair =
        PointToLineDistanceAndIntersection(circle_body.position,
                                           player_capsule.right_line.start,
                                           player_capsule.right_line.end);

    if (right_line_distance_pair.first != 0
        && right_line_distance_pair.first < circle_shape.radius) {
        // collided with right_line
        overlap = circle_shape.radius - right_line_distance_pair.first;
        push_direction = FumoVec2Normalize(right_line_distance_pair.second
                                           - circle_body.position);
        Collision collision {.overlap = overlap,
                             .push_direction = push_direction,
                             .intersection_point =
                                 right_line_distance_pair.second,
                             .distance = right_line_distance_pair.first,
                             .collided = true};

        // Collisions::calculate_collided_side(collision, player_capsule);

        return collision;
    }

    // -------------------------------------------------------------------------------
    // no collision happened
    return {overlap, push_direction};
}

Collision capsule_to_rect_collision_solving(Capsule& player_capsule,
                                            Body& player_body,
                                            const FumoRect& fumo_rect,
                                            const Body& fumo_rect_body) {
    // NOTE: assume fumo_rects are NOT rotated
    // (add rotation support later if i want to add slopes and stuff)
    // FumoRect doesnt store the position, that is stored in the Body
    // (this is useful for writing generic move functions)

    // -------------------------------------------------------------------------------
    // solve the collision
    // FIXME: current |
    // add a "sub_step_count"
    Collision collision = Collisions::CapsuleToRectCollision(player_capsule,
                                                             player_body,
                                                             fumo_rect,
                                                             fumo_rect_body);

    if (!collision.collided) {
        // no collision happened
        return collision;
    }

    // -------------------------------------------------------------------------------
    // points towards the player_body
    FumoVec2 push = collision.push_direction;
    float correction = 1.0f; // this value is here to stabilize the simulation
    push = FumoVec2Scale(push, collision.overlap * correction);
    player_body.position += push;
    player_capsule.update_capsule_positions(player_body);
    return collision;
}

Collision capsule_to_circle_collision_solving(Capsule& player_capsule,
                                              Body& player_body,
                                              const Circle& circle_shape,
                                              const Body& circle_body,
                                              const int& substep_count) {
    // -------------------------------------------------------------------------------
    // solve the collision
    Collision collision = Collisions::CapsuleToCircleCollision(player_capsule,
                                                               player_body,
                                                               circle_shape,
                                                               circle_body);
    if (!collision.collided) {
        // no collision happened
        return collision;
    }
    // -------------------------------------------------------------------------------
    // points towards the player_body
    FumoVec2 push = collision.push_direction;
    float correction = 1.0f; // this value is here to stabilize the simulation
    push = FumoVec2Scale(push, collision.overlap * correction);
    player_body.position += push;
    player_capsule.update_capsule_positions(player_body);
    return collision;
}

// [[nodiscard]] Collision LineToCapsuleCollision(const Capsule& player_capsule,
//                                                const Body& player_body,
//                                                const Line& line,
//                                                const Body& line_body) {
//     return LineToCapsuleCollision(
//         player_capsule,
//         player_body,
//         {line.start + line_body.position, line.end + player_body.position});
// }

// [[nodiscard]] Collision LineToCapsuleCollision(const Capsule& player_capsule,
//                                                const Body& player_body,
//                                                const Line& line) {
//
//     Collision closest_collision;
//     LINE_TO_SIDE_COLLISION(line, player_capsule.left_line, SHAPE::Capsule);
//     LINE_TO_SIDE_COLLISION(line, player_capsule.right_line, SHAPE::Capsule);
//
//     const FumoVec2 closest_intersection =
//         ClosestCircleLineIntersection(player_body.position,
//                                       player_capsule.radius,
//                                       line);
//
//     if (closest_intersection != FumoVec2 {0.0f, 0.0f}) {
//         float buffer = 0.1f;
//         // distance from each end of the line should add up to the length
//         // if we are on the line
//         float LineLength = FumoVec2Distance(line.start, line.end);
//         float d1 = FumoVec2Distance(closest_intersection, line.start);
//         float d2 = FumoVec2Distance(closest_intersection, line.end);
//
//         if (d1 + d2 >= LineLength - buffer && d1 + d2 <= LineLength + buffer) {
//             closest_collision.intersection_point = closest_intersection;
//             closest_collision.push_direction =
//                 FumoVec2Normalize(player_body.position - closest_intersection);
//             closest_collision.normal = closest_collision.push_direction;
//             closest_collision.collided = true;
//             closest_collision.distance =
//                 FumoVec2Distance(line.start, closest_intersection);
//             closest_collision.overlap =
//                 FumoVec2Distance(line.end,
//                                  closest_collision.intersection_point);
//             closest_collision.collided_shape = SHAPE::Capsule;
//         }
//         // not all collision information is guaranteed to be filled
//     }
//
//     return closest_collision;
// }
[[nodiscard]] bool LineToCapsuleCollided(const Capsule& player_capsule,
                                         const Body& player_body,
                                         const Line& line,
                                         const Body& line_body) {
    // lines store their real position in the body (kinda bad but its fine)
    return LineToCapsuleCollided(player_capsule,
                                 player_body,
                                 Line {.start = line.start + line_body.position,
                                       .end = line.end + line_body.position});
}

[[nodiscard]] bool LineToCapsuleCollided(const Capsule& player_capsule,
                                         const Body& player_body,
                                         const Line& line) {
    // assumes that the line points correspond to the real positions
    // most likely requires to add the body's position to this line

    return (LineToLineCollided(line, player_capsule.left_line)
            || LineToLineCollided(line, player_capsule.right_line)
            || LineToCircleCollision(
                   line,
                   Circle {.radius = player_capsule.radius},
                   Body {.position = player_capsule.bottom_circle_center})
                   .collided
            || LineToCircleCollision(
                   line,
                   Circle {.radius = player_capsule.radius},
                   Body {.position = player_capsule.top_circle_center})
                   .collided);
}
} // namespace Collisions
