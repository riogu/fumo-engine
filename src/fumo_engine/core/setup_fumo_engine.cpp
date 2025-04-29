#include "constants/planet_constants.hpp"
#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_engine/level_editor/level_editor.hpp"
#include "objects/factory_systems/factory_systems.hpp"
#include "objects/player_systems/player_general_systems.hpp"

namespace Initialization {
void initialize_directories();

}

void FumoEngine::setup_game() {
    SetTargetFPS(60);

    Initialization::initialize_directories();
    player_id = PlayerInitializer::initialize_player();

    fumo_camera = std::make_unique<FumoCamera>();
    const auto& player_body = ECS->get_component<Body>(player_id);
    // camera->target = player_body.position;
    fumo_camera->camera.target = screenCenter.to_raylib_vec2();
    fumo_camera->camera.rotation = 0.0f;
    fumo_camera->camera.zoom =
        engine_mode == EngineMode::LEVEL_EDITING ? 0.4f : 1.0f;
    fumo_camera->camera.offset = screenCenter.to_raylib_vec2();

    auto& player_animation = ECS->get_component<AnimationInfo>(player_id);
    AnimationPlayer::play(player_animation, "idle");

    // FumoRect collision_bounds {.x = player_body.position.x,
    //                            .y = player_body.position.y,
    //                            .width = RECT_WIDTH + MINIMUM_OBJECT_SIZE,
    //                            .height = RECT_HEIGHT + MINIMUM_OBJECT_SIZE};
    //
    // const auto& planet_factory = ECS->get_system<LevelEntityFactory>();
    // const auto& level_editor = ECS->get_system<DebugLevelEditor>();
    //
    // EntityId selected_rect_id =
    //     planet_factory->create_outline_rect(collision_bounds);
    // auto& selected_render = ECS->get_component<Render>(selected_rect_id);
    //
    // selected_render.color = FUMO_GOLD;
    //
    // ECS->entity_add_component(selected_rect_id, EditorSelectedObject {});
    //
    // level_editor->selection_rectangle_id = selected_rect_id;
}

namespace fs = std::filesystem;

// assume we have something like:
// build/
// | game_executable
// assets/
// serialized_data/
void Initialization::initialize_directories() {
    // TODO: only works on linux (add windows support later)
    fs::path curr_path = fs::current_path();

    if (*--curr_path.end() == "build") {
        // go up a dir name
        fs::current_path(curr_path.parent_path());
    }
    fs::create_directory("serialized_data");

    curr_path = fs::current_path();
    fs::current_path(curr_path / "serialized_data");
}
