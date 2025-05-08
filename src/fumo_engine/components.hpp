#pragma once
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>
#include <string_view>

#include "fumo_raylib.hpp"
#include "fumo_serialize_macros.hpp"
#include "raylib.h"

struct Line {
    FumoVec2 start {};
    FumoVec2 end {};

    void draw(const FumoColor& color, const FumoVec2& position) const;

    void _capsule_draw_line(const FumoColor& color) const;
    SERIALIZE(start, end)
};

// always run isnt used for fumo engine state, it used for registering systems

struct Body {

    float rotation {}; // in degrees
    FumoVec2 position = screenCenter;
    FumoVec2 velocity {0.0f, 0.0f};
    bool inverse_direction = false;
    // this code is horribly bad
    FumoVec2 real_gravity_direction = {0.0f, 1.0f};
    FumoVec2 real_x_direction = {-real_gravity_direction.y,
                                 real_gravity_direction.x};
    float current_gravity_strength {};

    FumoVec2 dash_gravity_direction = {0.0f, 1.0f};
    FumoVec2 dash_x_direction = {-dash_gravity_direction.y,
                                 dash_gravity_direction.x};

    // player events and state

    // NOTE: follows the gravity direction, not the vertical player direction
    [[nodiscard]] FumoVec2 get_dash_y_velocity() {
        return dash_gravity_direction
            * FumoVec2DotProduct(velocity, dash_gravity_direction);
    }

    [[nodiscard]] float get_dash_dot_y_velocity() {
        return FumoVec2DotProduct(velocity, dash_gravity_direction);
    }

    [[nodiscard]] float get_dash_dot_x_velocity() {
        return FumoVec2DotProduct(velocity, dash_x_direction);
    }

    [[nodiscard]] FumoVec2 get_dash_x_velocity() {
        return dash_x_direction * FumoVec2DotProduct(velocity, dash_x_direction);
    }

    // -------------------------------------------------------------------------------

    [[nodiscard]] FumoVec2 get_real_y_velocity() {
        return real_gravity_direction
            * FumoVec2DotProduct(velocity, real_gravity_direction);
    }

    [[nodiscard]] float get_real_dot_y_velocity() {
        return FumoVec2DotProduct(velocity, real_gravity_direction);
    }

    [[nodiscard]] FumoVec2 get_real_x_velocity() {
        return real_x_direction * FumoVec2DotProduct(velocity, real_x_direction);
    }

    // void scale_velocity(float scale) { velocity += gravity_direction * scale; }

    SERIALIZE(rotation,
              position,
              velocity,
              dash_gravity_direction,
              dash_x_direction,
              inverse_direction);
};

struct Circle {
    float radius;
    void draw(const FumoColor& color, const FumoVec2& position) const;
    // void resize(const FumoVec2& mouse_position, const FumoVec2& body_position);
    SERIALIZE(radius)
};

struct OutlineRect {
    FumoRect outline_rect;
    void draw(const FumoColor& color, const FumoVec2& position) const;
    // void resize(const FumoVec2& mouse_position, const FumoVec2& body_position);
    SERIALIZE(outline_rect)
};

struct Capsule {

    float radius;
    FumoVec2 top_circle_center;
    FumoVec2 bottom_circle_center;

    // PERF: should remove the capsule lines later on if need optimization
    Line left_line; // .start is the bottom point
    Line right_line; // .start is the bottom point
    Line middle_line; // starts on the bottom ends on the top of the capsule

    void update_capsule_positions(const Body& player_body) {
        // this is all peidro's fault his code is beyond horrible
        top_circle_center =
            player_body.position - player_body.real_gravity_direction * radius;
        bottom_circle_center =
            player_body.position + player_body.real_gravity_direction * radius;

        left_line.end = top_circle_center - player_body.real_x_direction * radius;
        left_line.start =
            bottom_circle_center - player_body.real_x_direction * radius;

        right_line.end =
            top_circle_center + player_body.real_x_direction * radius;
        right_line.start =
            bottom_circle_center + player_body.real_x_direction * radius;

        middle_line.start =
            bottom_circle_center + player_body.real_gravity_direction * radius;
        middle_line.end =
            top_circle_center - player_body.real_gravity_direction * radius;
    }

    void draw(const FumoColor& color, const FumoVec2& position) const;
    SERIALIZE(radius,
              top_circle_center,
              bottom_circle_center,
              left_line,
              right_line,
              middle_line);
};

// NOTE: we only want a single side for now, add a fumo_rect version
// later
// WARNING: **NOT** counted from the surface of the object we are on
struct ParallelGravityField {
    // this is parallel to a surface and points in one direction
    // the field has one direction INSIDE this fumo_rect

    // NOTE: assume the field points towards the bottom side of the fumo_rect

    // NOTE: there is no body for gravity fields because we want to have the body
    // be used for the floor and not for the field on planets
    FumoRect field_fumo_rect {};

    FumoVec2 gravity_direction = {0.0f, 1.0f}; // default is vertical
    float gravity_strength {};
    float rotation {}; // in degrees
    // extern std::unique_ptr<GlobalState> global;
    // FumoVec2 position = screenCenter;

    bool is_inside_field(const Body& player_body,
                         const Capsule& player_shape,
                         const Body& parallel_body) const;
    void update_gravity(Body& player_body);

    void draw(const FumoColor& color, const FumoVec2& position) const;
    // void resize(const FumoVec2& mouse_position, const FumoVec2& body_position);

    SERIALIZE(field_fumo_rect, gravity_direction, gravity_strength, rotation)
};

// WARNING: **NOT** counted from the surface of the object we are on
struct CircularGravityField {
    // FIXME: save the push on the grav fields themselves later when refactoring
    // FumoVec2 push_direction;
    float radius;
    float gravity_strength;
    FumoVec2 position = screenCenter;
    FumoVec2 gravity_direction;

    bool is_inside_field(const Body& player_body,
                         const Capsule& player_shape,
                         const Body& circular_body) const;
    void update_gravity(Body& player_body, const Body& body_planet);

    void draw(const FumoColor& color, const FumoVec2& position) const;
    // void resize(const FumoVec2& mouse_position, const FumoVec2& body_position);
    SERIALIZE(radius, gravity_strength, position, gravity_direction)
};

// NOTE: to make a really modular and reusable timer class,
// consider using std::function to send in a function call
// after the timer ends

struct Timer {
    friend struct TimerHandler;

    float seconds_duration;
    std::string system_name = "NO_NAME";

    void make_timer(float _duration_seconds, std::string_view _system_name) {
        seconds_duration = _duration_seconds;
        system_name = _system_name;

        starting_time = GetTime();
        ending_time = starting_time + seconds_duration;
    }

    SERIALIZE(seconds_duration, system_name);
    // another templated constructor we might wanna use in the future
    // template<typename T>
    // Timer(float duration_seconds) : duration_seconds(duration_seconds) {
    //     system_name = libassert::type_name<T>();
    //
    //     starting_time = GetTime();
    //     ending_time = starting_time + duration_seconds;
    // }

  private:
    float starting_time;
    float ending_time;
};

struct AnimationInfo {

    // keeps track of an entity's animation
    // so we can update that entity's SpriteSheet2D
    // a vector to hold the animations we want to play for the entity
    std::vector<std::string> sheet_vector {std::string("NO_SHEET")};
    // NOTE: it is a bit of overhead, but i am following godot's implementation
    // for this. for now i do not need this kind of behavior, but i will for stuff
    // like a landing animation being queued after a jump.
    // we also want to be able to replace the head element
    int frame_progress {1}; // progress along the frame
    int frame_speed {}; // number of frames to show per second
    int sprite_frame_count {};
    std::string current_sheet_name = "NO_SHEET";
    FumoRect current_region_rect {};
    int sub_counter {};
    bool is_running = false;

    float sprite_scaling {};

    SERIALIZE(sheet_vector,
              frame_progress,
              frame_speed,
              sprite_frame_count,
              current_sheet_name,
              current_region_rect,
              sub_counter,
              is_running,
              sprite_scaling)
};

// ------------------------------------------------------------------------
// NOTE: not used as components
// ------------------------------------------------------------------------

// NOTE: when making an animation, we created an EntityId for that animation,
// which we then attribute to whatever entity should hold that animation

struct SpriteSheet2D {
    // used to animate entities
    Texture2D texture_sheet;
    std::string sprite_sheet_name;
    int sprite_frame_count;
    int base_frame_speed {}; // number of frames to show per second
    FumoRect base_region_rect {.x = 0.0f,
                               .y = 0.0f,
                               .width = (float)texture_sheet.width
                                   / sprite_frame_count,
                               .height = (float)texture_sheet.height};
    // base rect that defines how we go through each animation frame
    bool looping = false;

    SERIALIZE(sprite_sheet_name,
              sprite_frame_count,
              base_frame_speed,
              base_region_rect,
              looping)
};

struct Sprite2D {
    Texture2D texture;
    std::string_view sprite_name;
    FumoRect region_rect;

    SERIALIZE(sprite_name, region_rect)
};
