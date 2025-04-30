#include "fumo_engine/core/fumo_engine.hpp"
extern std::unique_ptr<FumoEngine> fumo_engine;
#include "fumo_engine/level_editor/level_editor.hpp"
#include "fumo_engine/serialization/fumo_serializer.hpp"

void DebugLevelEditor::handle_input() {
    fumo_engine->fumo_camera->camera.zoom +=
        ((float)GetMouseWheelMove() * 0.05f);
    if (fumo_engine->fumo_camera->camera.zoom < 0.05f)
        fumo_engine->fumo_camera->camera.zoom = 0.05f;

    FumoVec2 mouse_position =
        to_fumo_vec2(GetScreenToWorld2D(GetMousePosition(),
                                        fumo_engine->fumo_camera->camera));

    // DrawCircleLinesV(GetMousePosition(), mouse_radius, GREEN);

    find_selection(mouse_position);
    // if (IsKeyPressed(KEY_P)) reset_position();
    // FIXME: add spawning grav fields in all directions in rectangles
    if (IsKeyPressed(KEY_Q)) fumo_engine->ECS->debug_print_scheduler();
    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_S)) save_level();
    if (IsKeyPressed(KEY_D)) delete_planet(mouse_position);
    if (IsKeyDown(KEY_LEFT_ALT)) move_screen_to_mouse(mouse_position);
    if (IsKeyPressed(KEY_F1)) spawn_circle(mouse_position);
    if (IsKeyPressed(KEY_F2)) spawn_circular_field(mouse_position);
    if (IsKeyPressed(KEY_F3)) spawn_rect(mouse_position);
    if (IsKeyPressed(KEY_F4)) spawn_rect_field(mouse_position);
    if (IsKeyPressed(KEY_F5)) spawn_transition_line(mouse_position);
}

//---------------------------------------------------------
// generic functionality

void DebugLevelEditor::save_level() {
    const auto& level_serializer =
        fumo_engine->ECS->get_system<LevelSerializer>();
    // level_serializer->deserialize_levels();
    level_serializer->serialize_levels();
}
