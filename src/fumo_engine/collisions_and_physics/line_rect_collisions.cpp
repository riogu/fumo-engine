#include "fumo_engine/collisions_and_physics/collision_functions.hpp"

namespace Collisions {

[[nodiscard]] bool LineToRectCollided(const Line& line,
                                      const FumoRect& rect,
                                      const Body& rect_body) {

    FumoVec2 TopLeft = {rect_body.position.x, rect_body.position.y};
    FumoVec2 TopRight = {rect_body.position.x + rect.width,
                         rect_body.position.y};
    FumoVec2 BottomLeft = {rect_body.position.x,
                           rect_body.position.y + rect.height};
    FumoVec2 BottomRight = {rect_body.position.x + rect.width,
                            rect_body.position.y + rect.height};

    const Line left_side {TopLeft, BottomLeft},
        right_side {TopRight, BottomRight}, top_side {TopLeft, TopRight},
        bottom_side {BottomLeft, BottomRight};

    return (LineToLineCollided(line, left_side)
            || LineToLineCollided(line, right_side)
            || LineToLineCollided(line, top_side)
            || LineToLineCollided(line, bottom_side));
}

[[nodiscard]] const Collision LineToRectCollision(const Line& line,
                                                  const FumoRect& rect,
                                                  const Body& rect_body) {
    Collision closest_collision;

    FumoVec2 TopLeft = {rect_body.position.x, rect_body.position.y};
    FumoVec2 TopRight = {rect_body.position.x + rect.width,
                         rect_body.position.y};
    FumoVec2 BottomLeft = {rect_body.position.x,
                           rect_body.position.y + rect.height};
    FumoVec2 BottomRight = {rect_body.position.x + rect.width,
                            rect_body.position.y + rect.height};

    const Line left_side {TopLeft, BottomLeft},
        right_side {TopRight, BottomRight}, top_side {TopLeft, TopRight},
        bottom_side {BottomLeft, BottomRight};

    // NOTE: if we add rotations, you need to replace FumoVec2Snap4Directions()
    // with actually finding the normal to the surface

    // PRINT(collision.normal.x);
    // PRINT(collision.normal.y)
    // print_direction(vector_to_direction(collision.normal));

    LINE_TO_SIDE_COLLISION(line, left_side, SHAPE::Rectangle);
    LINE_TO_SIDE_COLLISION(line, right_side, SHAPE::Rectangle);
    LINE_TO_SIDE_COLLISION(line, top_side, SHAPE::Rectangle);
    LINE_TO_SIDE_COLLISION(line, bottom_side, SHAPE::Rectangle);

    return closest_collision;
}

} // namespace Collisions
