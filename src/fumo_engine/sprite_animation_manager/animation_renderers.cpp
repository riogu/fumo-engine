#include <cstdlib>

#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_engine/sprite_animation_manager/sprite_and_animation_systems.hpp"
#include "raylib.h"
#include "sprite_and_animation_systems.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

void AnimationRenderer::draw_animations() {
    for (auto entity_id : sys_entities) {
        const auto& animation_info =
            fumo_engine->ECS->get_component<AnimationInfo>(entity_id);

        const auto& sheet_texture =
            fumo_engine->sprite_manager->get_sprite_texture(
                animation_info.current_sheet_name);

        const auto& entity_body =
            fumo_engine->ECS->get_component<Body>(entity_id);

        draw_animation(animation_info, sheet_texture, entity_body);
    }
}

void AnimationRenderer::draw_animation(const AnimationInfo& animation_info,
                                       const Texture2D& sheet_texture,
                                       const Body& body) {
    // NOTE:
    // destination.x/y -> where to draw on the screen
    // origin -> where do we center the fumo_rect itself according to destination.x/y
    // origin is relative to the destination fumo_rect
    // width and height provide the scaling of our texture

    // PRINT("GAMING");
    // debug_print_animation_info(animation_info);

    BeginMode2D(fumo_engine->fumo_camera->camera);

    auto& player_state =
        fumo_engine->ECS->get_component<EntityState>(fumo_engine->player_id);

    FumoRect destination = {body.position.x,
                            body.position.y,
                            animation_info.current_region_rect.width
                                * animation_info.sprite_scaling,
                            animation_info.current_region_rect.height
                                * animation_info.sprite_scaling};

    FumoRect source = animation_info.current_region_rect;

    if (body.inverse_direction) {

        source = {animation_info.current_region_rect.x,
                  animation_info.current_region_rect.y,
                  -animation_info.current_region_rect.width,
                  animation_info.current_region_rect.height};
    }

    FumoVec2 origin = {abs(destination.width / 2), 3 * destination.height / 4};

    DrawTexturePro(sheet_texture,
                   source.to_raylib_rect(),
                   destination.to_raylib_rect(),
                   origin.to_raylib_vec2(),
                   body.rotation,
                   WHITE);

    EndMode2D();
}
