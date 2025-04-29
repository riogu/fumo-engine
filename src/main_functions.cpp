#include "main_functions.hpp"

#include "constants/planet_constants.hpp"
#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_raylib.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

// bool IsKeyStillDown(int key) {
//     bool still_down = false;
//
//     if ((key > 0) && (key < MAX_KEYBOARD_KEYS))
//     {
//         if ((CORE.Input.Keyboard.previousKeyState[key] == 1) && (CORE.Input.Keyboard.currentKeyState[key] == 1)) still_down = true;
//     }
//
//     return still_down;
//
// }

void PRINT_COMPONENT_NAME(const ComponentId& component_id) {
#define XMACRO(Type) \
    case AllComponentTypes::Type: \
        PRINT_NO_NAME(#Type) \
        break;

    switch (AllComponentTypes {component_id}) { ALL_COMPONENTS_X_MACRO() }

#undef XMACRO // NOTE: dont forget to undefine the xmacro
}

void debug_print_animation_info(const AnimationInfo& animation_info) {
    PRINT(animation_info.current_region_rect.x);
    PRINT(animation_info.current_region_rect.y);
    // PRINT(animation_info.current_region_rect.width);
    // PRINT(animation_info.current_region_rect.height);
    PRINT(animation_info.current_sheet_name);
    PRINT(animation_info.frame_progress);
    std::cerr << '\n';
}

void UpdateCameraCenterSmoothFollow(Camera2D* camera,
                                    const FumoVec2& position) {
    float minSpeed = 30;
    float minEffectLength = 10;
    float fractionSpeed = 0.8f;

    camera->offset = {screenWidth / 2.0f, screenHeight / 2.0f};
    FumoVec2 diff = FumoVec2Subtract(position, to_fumo_vec2(camera->target));
    float length = FumoVec2Length(diff);

    float scaling = 8.0f;

    if (length > minEffectLength) {
        float speed = fmaxf(fractionSpeed * length, minSpeed);
        camera->target =
            FumoVec2Add(
                to_fumo_vec2(camera->target),
                FumoVec2Scale(diff, speed * fumo_engine->frametime / length)
                    * scaling)
                .to_raylib_vec2();
    }
}

void debug_player_drawing(const Capsule& player_capsule,
                          const Body& player_body) {
    // extra visualization code
    BeginMode2D(fumo_engine->fumo_camera->camera);
    const auto& render =
        fumo_engine->ECS->get_component<Render>(fumo_engine->player_id);
    DrawCircleV(player_capsule.bottom_circle_center.to_raylib_vec2(),
                player_capsule.radius,
                render.color.to_raylib_color());

    DrawCircleV(player_capsule.top_circle_center.to_raylib_vec2(),
                player_capsule.radius,
                render.color.to_raylib_color());
    DrawLineEx(player_capsule.left_line.start.to_raylib_vec2(),
               player_capsule.left_line.end.to_raylib_vec2(),
               4.0f,
               render.color.to_raylib_color());
    DrawLineEx(player_capsule.right_line.start.to_raylib_vec2(),
               player_capsule.right_line.end.to_raylib_vec2(),
               4.0f,
               render.color.to_raylib_color());

    DrawLineEx(player_capsule.middle_line.start.to_raylib_vec2(),
               player_capsule.middle_line.end.to_raylib_vec2(),
               4.0f,
               render.color.to_raylib_color());

    // DrawCircleV(player_capsule.bottom_circle_center.to_raylib_vec2(),
    //             player_capsule.radius,
    //             {static_cast<unsigned char>(render.color.r + 90),
    //              static_cast<unsigned char>(render.color.g + 40),
    //              render.color.b,
    //              static_cast<unsigned char>(render.color.a + 105)});
    // DrawCircleV(player_capsule.top_circle_center.to_raylib_vec2(),
    //             player_capsule.radius,
    //             {static_cast<unsigned char>(render.color.r + 90),
    //              static_cast<unsigned char>(render.color.g + 40),
    //              render.color.b,
    //              static_cast<unsigned char>(render.color.a + 105)});
    // DrawLineEx(player_capsule.left_line.start.to_raylib_vec2(),
    //            player_capsule.left_line.end.to_raylib_vec2(),
    //            4.0f,
    //            {static_cast<unsigned char>(render.color.r + 90),
    //             static_cast<unsigned char>(render.color.g + 40),
    //             render.color.b,
    //             static_cast<unsigned char>(render.color.a + 105)});
    // DrawLineEx(player_capsule.right_line.start.to_raylib_vec2(),
    //            player_capsule.right_line.end.to_raylib_vec2(),
    //            4.0f,
    //            {static_cast<unsigned char>(render.color.r + 90),
    //             static_cast<unsigned char>(render.color.g + 40),
    //             render.color.b,
    //             static_cast<unsigned char>(render.color.a + 105)});
    // double gravity_reach = 300.0f;
    // FumoVec2 normalized_velocity = FumoVec2Normalize(player_body.velocity);
    // FumoVec2 line_end = player_body.position + normalized_velocity * gravity_reach;
    // if (player_state.jumping) {
    //     line_end =
    //         player_body.position - (player_body.gravity_direction) * gravity_reach;
    // }
    // DrawLineV(player_body.position, line_end, YELLOW);
    EndMode2D();
}

void weird_debug_print(std::pair<float, FumoVec2> closest_pair,
                       std::pair<float, FumoVec2> bottom_pair,
                       std::pair<float, FumoVec2> left_pair,
                       std::pair<float, FumoVec2> right_pair,
                       std::pair<float, FumoVec2> top_pair) {
    // -------------------------------------------------------------------------------
    // DEBUG
    BeginMode2D(fumo_engine->fumo_camera->camera);

    if (closest_pair.first == 0.0f) {
    } else if (closest_pair.first == bottom_pair.first) {
        DrawCircleV(bottom_pair.second.to_raylib_vec2(),
                    CIRCLE_RADIUS,
                    MAGENTA);
    } else if (closest_pair.first == left_pair.first) {
        DrawCircleV(left_pair.second.to_raylib_vec2(), CIRCLE_RADIUS, ORANGE);
    } else if (closest_pair.first == right_pair.first) {
        DrawCircleV(right_pair.second.to_raylib_vec2(), CIRCLE_RADIUS, PURPLE);
    } else if (closest_pair.first == top_pair.first) {
        DrawCircleV(top_pair.second.to_raylib_vec2(), CIRCLE_RADIUS, SKYBLUE);
    }
    EndMode2D();
}
