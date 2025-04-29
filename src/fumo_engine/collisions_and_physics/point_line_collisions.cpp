#include "fumo_engine/collisions_and_physics/collision_functions.hpp"
#include "fumo_engine/core/fumo_engine.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

namespace Collisions {

[[nodiscard]] std::pair<float, FumoVec2>
PointToLineDistanceAndIntersection(const FumoVec2& Point,
                                   const FumoVec2& LineStart,
                                   const FumoVec2& LineEnd) {
    // draw a perpendicular line from the point towards the input line
    // Example:   LineStart
    //               |
    //               |
    // (Intersection)*------------ * (Point)
    //               |
    //               |
    //               |
    //            LineEnd
    //
    // If the point falls along the line, return intersection and distance
    // otherwise, return {0, {0.0f, 0.0f}}
    //
    //
    float LineMag;
    FumoVec2 Intersection;

    LineMag = FumoVec2Distance(LineStart, LineEnd);

    FumoVec2 line_vector = LineEnd - LineStart;
    FumoVec2 line_start_to_point = Point - LineStart;
    float dot = FumoVec2DotProduct(line_start_to_point, line_vector)
        / (LineMag * LineMag);

    Intersection.x = LineStart.x + (dot * (LineEnd.x - LineStart.x));
    Intersection.y = LineStart.y + (dot * (LineEnd.y - LineStart.y));

    // --------------------------------------------------------------------
    // check if its along the line
    float buffer = 0.1f;
    float d1 = FumoVec2Distance(Intersection, LineStart);
    float d2 = FumoVec2Distance(Intersection, LineEnd);
    if (d1 + d2 >= LineMag - buffer && d1 + d2 <= LineMag + buffer) {
        float Distance = FumoVec2Distance(Point, Intersection);
        // --------------------------------------------------------------------
        // find the distance from the point to the line intersection
        // --------------------------------------------------------------------
        return {Distance, Intersection};
    }

    return {0, {0.0f, 0.0f}}; // closest point does not fall along the line
}

[[nodiscard]] Collision PointToLineCollision(const FumoVec2& Point,
                                             const Line& line) {

    Collision collision {};

    float LineLength;
    FumoVec2 Intersection;
    FumoVec2 LineStart = line.start;
    FumoVec2 LineEnd = line.end;

    LineLength = FumoVec2Distance(LineStart, LineEnd);

    FumoVec2 line_vector = LineEnd - LineStart;
    FumoVec2 line_start_to_point = Point - LineStart;
    float dot = FumoVec2DotProduct(line_start_to_point, line_vector)
        / (LineLength * LineLength);

    Intersection.x = LineStart.x + (dot * (LineEnd.x - LineStart.x));
    Intersection.y = LineStart.y + (dot * (LineEnd.y - LineStart.y));

    // --------------------------------------------------------------------
    // check if its along the line
    float buffer = 0.1f;
    // distance from each end of the line should add up to the length
    // if we are on the line
    float d1 = FumoVec2Distance(Intersection, LineStart);
    float d2 = FumoVec2Distance(Intersection, LineEnd);
    if (d1 + d2 >= LineLength - buffer && d1 + d2 <= LineLength + buffer) {
        float Distance = FumoVec2Distance(Point, Intersection);
        // --------------------------------------------------------------------
        // find the distance from the point to the line intersection
        // --------------------------------------------------------------------
        collision.distance = Distance;
        collision.intersection_point = Intersection;
        collision.collided = true;

        return collision;
    }

    return collision; // closest point does not fall along the line
}

[[nodiscard]] FumoVec2 closest_point(const FumoVec2 target,
                                     const std::vector<FumoVec2>& points) {
    FumoVec2 closest_point {6969, 6969};
    float min_distance {696969};

    for (const auto& point : points) {
        float dist = FumoVec2Distance(target, point);
        if (dist < min_distance) {
            closest_point = point;
            min_distance = dist;
        }
    }
    return closest_point;
}

[[nodiscard]] bool LineToLineCollided(const Line& line1, const Line& line2) {

    // calculate the distance to intersection point

    // FumoVec2 intersection {0.0f, 0.0f};

    float uA =
        ((line2.end.x - line2.start.x) * (line1.start.y - line2.start.y)
         - (line2.end.y - line2.start.y) * (line1.start.x - line2.start.x))
        / ((line2.end.y - line2.start.y) * (line1.end.x - line1.start.x)
           - (line2.end.x - line2.start.x) * (line1.end.y - line1.start.y));

    float uB =
        ((line1.end.x - line1.start.x) * (line1.start.y - line2.start.y)
         - (line1.end.y - line1.start.y) * (line1.start.x - line2.start.x))
        / ((line2.end.y - line2.start.y) * (line1.end.x - line1.start.x)
           - (line2.end.x - line2.start.x) * (line1.end.y - line1.start.y));
    // if uA and uB are between 0-1, lines are colliding
    float buffer = 0.1f;
    return (uA >= 0 && uA <= 1 + buffer && uB >= 0 && uB <= 1 + buffer);
}

[[nodiscard]] Collision LineToLineCollision(const Line& line1,
                                            const Line& line2) {

    // calculate the distance to intersection point

    FumoVec2 intersection {0.0f, 0.0f};
    Collision collision;

    float uA =
        ((line2.end.x - line2.start.x) * (line1.start.y - line2.start.y)
         - (line2.end.y - line2.start.y) * (line1.start.x - line2.start.x))
        / ((line2.end.y - line2.start.y) * (line1.end.x - line1.start.x)
           - (line2.end.x - line2.start.x) * (line1.end.y - line1.start.y));

    float uB =
        ((line1.end.x - line1.start.x) * (line1.start.y - line2.start.y)
         - (line1.end.y - line1.start.y) * (line1.start.x - line2.start.x))
        / ((line2.end.y - line2.start.y) * (line1.end.x - line1.start.x)
           - (line2.end.x - line2.start.x) * (line1.end.y - line1.start.y));
    // if uA and uB are between 0-1, lines are colliding
    float buffer = 0.0f;

    // if uA and uB are between 0-1, lines are colliding
    if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
        intersection.x = line1.start.x + (uA * (line1.end.x - line1.start.x));
        intersection.y = line1.start.y + (uA * (line1.end.y - line1.start.y));

        collision.intersection_point = intersection;
        collision.collided = true;
    }
    return collision;
}

} // namespace Collisions

// [[nodiscard]] float PointToLineDistance(const FumoVec2& Point, const FumoVec2&
// LineStart,
//                                         const FumoVec2& LineEnd) {
//     float LineMag;
//     float U;
//     FumoVec2 Intersection;
//
//     LineMag = FumoVec2Distance(LineEnd, LineStart);
//
//     U = (((Point.x - LineStart.y) * (LineEnd.x - LineStart.x)) +
//          ((Point.y - LineStart.y) * (LineEnd.x - LineStart.y))) /
//         (LineMag * LineMag);
//
//     if (U < 0.0f || U > 1.0f)
//         return 0; // closest point does not fall along the line
//
//     Intersection.x = LineStart.x + U * (LineEnd.x - LineStart.x);
//     Intersection.y = LineStart.y + U * (LineEnd.y - LineStart.y);
//
//     // --------------------------------------------------------------------
//     // find the distance from the point to the line intersection
//     // --------------------------------------------------------------------
//
//     float Distance = FumoVec2Distance(Point, Intersection);
//
//     return Distance;
// }
