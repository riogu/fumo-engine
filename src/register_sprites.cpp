#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_engine/sprite_animation_manager/sprite_and_animation_systems.hpp"
#include "initialization.hpp"
#include "raylib.h"

extern std::unique_ptr<FumoEngine> fumo_engine;

// specific segmentation of loads
namespace Initialization {

void initialize_all_textures() { load_player_textures(); }

void load_player_textures() {
    // FIXME: change the way we open the files to use std::filesystem::path
    // and write it so that it will work when sending the .exe
    // (find current path, then load assets... etc)
    fumo_engine->sprite_manager->register_sprite(SpriteSheet2D {
        .texture_sheet = LoadTexture("assets/test_player/dash_short.png"),
        .sprite_sheet_name = "dash",
        .sprite_frame_count = 5,
        .base_frame_speed = 4});

    fumo_engine->sprite_manager->register_sprite(SpriteSheet2D {
        .texture_sheet = LoadTexture("assets/test_player/jump.png"),
        .sprite_sheet_name = "jump",
        .sprite_frame_count = 4,
        .base_frame_speed = 8});

    fumo_engine->sprite_manager->register_sprite(SpriteSheet2D {
        .texture_sheet = LoadTexture("assets/test_player/landing.png"),
        .sprite_sheet_name = "landing",
        .sprite_frame_count = 2,
        .base_frame_speed = 8});

    fumo_engine->sprite_manager->register_sprite(SpriteSheet2D {
        .texture_sheet = LoadTexture("assets/test_player/idle.png"),
        .sprite_sheet_name = "idle",
        .sprite_frame_count = 7,
        .base_frame_speed = 8,
        .looping = true});

    fumo_engine->sprite_manager->register_sprite(SpriteSheet2D {
        .texture_sheet = LoadTexture("assets/test_player/run.png"),
        .sprite_sheet_name = "run",
        .sprite_frame_count = 8,
        .base_frame_speed = 6,
        .looping = true});

    auto back_sheet = LoadTexture("assets/test_player/run.png");

    fumo_engine->sprite_manager->register_sprite(SpriteSheet2D {
        .texture_sheet = back_sheet,
        .sprite_sheet_name = "run_backwards",
        .sprite_frame_count = 8,
        .base_frame_speed = 6,
        .base_region_rect = {.x = 0.0f,
                             .y = 0.0f,
                             .width = (float)back_sheet.width / -8.0f,
                             .height = (float)back_sheet.height},
        .looping = true});
}
} // namespace Initialization

// void func() {
//     // AnimationInfo parameters are optional and we should only touch the
//     // "frame_speed" variable for customizing the animation
//     fumo_engine->ECS->entity_add_components(fumo_engine->player_id, AnimationInfo{});
//
//     const auto& animation_player = fumo_engine->ECS->get_system<AnimationPlayer>();
//
//     AnimationPlayer::play(fumo_engine->ECS->get_component<AnimationInfo>(fumo_engine->player_id),
//                            "scarfy");
// }
