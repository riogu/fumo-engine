#include "fumo_engine/core/fumo_engine.hpp"
#include "objects/player_systems/player_general_systems.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

EntityId PlayerInitializer::initialize_player() {
    EntityId player_id = fumo_engine->ECS->create_entity();

    // fumo_engine->ECS->entity_add_components(player_id, PlayerFlag{});
    fumo_engine->ECS->entity_add_components(
        player_id,
        EntityState {.can_swap_orbits = true});
    fumo_engine->ECS->entity_add_components(
        player_id,
        Render {.color = {50, 50, 50, 150}});

    fumo_engine->ECS->entity_add_components(
        player_id,
        AnimationInfo {.sprite_scaling = 2.5f});

    Body player_body {
        .position = {.x = screenWidth / 2.0f, .y = screenCenter.y},
        .velocity = {0.0f, 0.0f}};

    fumo_engine->ECS->entity_add_components(player_id, player_body);

    Capsule player_capsule {.radius = 26.0f};
    player_capsule.update_capsule_positions(player_body);
    fumo_engine->ECS->entity_add_components(player_id, player_capsule);
    fumo_engine->ECS->entity_add_components(player_id, MovedEventData {});
    fumo_engine->ECS->entity_add_components(player_id, CollisionEventData {});

    return player_id;
}
