#include "constants/planet_constants.hpp"
#include "fumo_engine/components.hpp"
#include "fumo_engine/screen_components.hpp"

void FumoRect::draw_outline(const FumoColor& color,
                            const FumoVec2& position) const {
    FumoVec2 TopLeft = {position.x, position.y};
    FumoVec2 TopRight = {position.x + width, position.y};
    FumoVec2 BottomLeft = {position.x, position.y + height};
    FumoVec2 BottomRight = {position.x + width, position.y + height};
    FumoDrawLineEx(TopLeft, TopRight, LINE_THICKNESS, color);
    FumoDrawLineEx(TopLeft, BottomLeft, LINE_THICKNESS, color);
    FumoDrawLineEx(TopRight, BottomRight, LINE_THICKNESS, color);
    FumoDrawLineEx(BottomLeft, BottomRight, LINE_THICKNESS, color);
}

void FumoRect::draw(const FumoColor& color, const FumoVec2& position) const {
    FumoDrawRectV(position, {width, height}, color);
}

void Circle::draw(const FumoColor& color, const FumoVec2& position) const {
    FumoDrawCircleV(position, radius, color);
}

void CircularGravityField::draw(const FumoColor& color,
                                const FumoVec2& position) const {
    FumoDrawCircleV(position, radius, FumoColorAlpha(color, 0.2f));
}

void ParallelGravityField::draw(const FumoColor& color,
                                const FumoVec2& position) const {
    FumoDrawRectV(position,
                  {field_fumo_rect.width, field_fumo_rect.height},
                  FumoColorAlpha(color, 0.2f));
}

void Line::draw(const FumoColor& color, const FumoVec2& position) const {
    FumoDrawLineEx(start + position, end + position, LINE_THICKNESS, color);
}

void Line::_capsule_draw_line(const FumoColor& color) const {
    FumoDrawLineEx(start, end, LINE_THICKNESS, color);
}

void Capsule::draw(const FumoColor& color, const FumoVec2& position) const {
    left_line._capsule_draw_line(color);
    middle_line._capsule_draw_line(color);
    right_line._capsule_draw_line(color);
    FumoDrawCircleV(top_circle_center, radius, color);
    FumoDrawCircleV(bottom_circle_center, radius, color);
}

void ScreenTransitionLine::draw(const FumoColor& color,
                                const FumoVec2& position) const {
    transition_line.draw(color, position);
}

void OutlineRect::draw(const FumoColor& color, const FumoVec2& position) const {
    outline_rect.draw_outline(color, position);
}
