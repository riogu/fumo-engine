#include "constants/planet_constants.hpp"
#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_engine/screen_components.hpp"
#include "objects/factory_systems/factory_systems.hpp"
extern std::unique_ptr<FumoEngine> fumo_engine;

[[nodiscard]] FumoRect make_default_field_rect(FumoVec2 position);
[[nodiscard]] FumoRect make_default_ground_rect(FumoVec2 position);

EntityId LevelEntityFactory::create_screen_transition(FumoVec2 position) {

    EntityId entity_id = fumo_engine->ECS->create_entity();
    ScreenTransitionData screen_transition_rect {
        .transition_line = {.end = {0.0f, 150.0f}},
        .previous_screen =
            Screen {.screen_id = 0, .screen_position = screenCenter},
        .next_screen = Screen {.screen_id = 1,
                               .screen_position = {screenCenter.x + screenWidth,
                                                   screenCenter.y}},
        .transition_direction = DIRECTION::RIGHT};

    fumo_engine->ECS->entity_add_components(entity_id,
                                            screen_transition_rect,
                                            Render {.color = FUMO_BROWN},
                                            Body {.position = position},
                                            Serialized {0},
                                            LevelId {0});

    sys_entities.insert(entity_id);

    return entity_id;
}

EntityId LevelEntityFactory::create_outline_rect(FumoRect rect) {
    EntityId entity_id = fumo_engine->ECS->create_entity();

    fumo_engine->ECS->entity_add_components(entity_id,
                                            OutlineRect {.outline_rect = rect});

    fumo_engine->ECS->entity_add_components(entity_id,
                                            Render {.color = FUMO_GREEN});

    fumo_engine->ECS->entity_add_components(
        entity_id,
        Body {.position = {rect.x, rect.y}, .velocity = {0.0f, 0.0f}});

    fumo_engine->ECS->entity_add_components(entity_id, Serialized {0});
    fumo_engine->ECS->entity_add_components(entity_id, LevelId {0});

    sys_entities.insert(entity_id);

    return entity_id;
}

EntityId LevelEntityFactory::create_outline_rect(FumoVec2 position) {
    EntityId entity_id = fumo_engine->ECS->create_entity();

    FumoRect ground_fumo_rect = make_default_ground_rect(position);
    fumo_engine->ECS->entity_add_components(
        entity_id,
        OutlineRect {.outline_rect = {0.0f, 0.0f, screenWidth, screenHeight}});

    fumo_engine->ECS->entity_add_components(entity_id,
                                            Render {.color = FUMO_GREEN});

    fumo_engine->ECS->entity_add_components(
        entity_id,
        Body {.position = {ground_fumo_rect.x, ground_fumo_rect.y},
              .velocity = {0.0f, 0.0f}});

    fumo_engine->ECS->entity_add_components(entity_id, Serialized {0});
    fumo_engine->ECS->entity_add_components(entity_id, LevelId {0});

    sys_entities.insert(entity_id);

    return entity_id;
}

EntityId LevelEntityFactory::create_rect(FumoVec2 position) {
    EntityId entity_id = fumo_engine->ECS->create_entity();

    FumoRect ground_fumo_rect = make_default_ground_rect(position);
    fumo_engine->ECS->entity_add_components(entity_id, ground_fumo_rect);

    fumo_engine->ECS->entity_add_components(entity_id, ColliderObjectFlag {});
    fumo_engine->ECS->entity_add_components(entity_id,
                                            Render {.color = FUMO_RED});

    fumo_engine->ECS->entity_add_components(
        entity_id,
        Body {.position = {ground_fumo_rect.x, ground_fumo_rect.y},
              .velocity = {0.0f, 0.0f}});

    fumo_engine->ECS->entity_add_components(entity_id, Serialized {0});
    fumo_engine->ECS->entity_add_components(entity_id, LevelId {0});

    sys_entities.insert(entity_id);

    return entity_id;
}

EntityId LevelEntityFactory::create_rect_field(FumoVec2 position) {
    EntityId entity_id = fumo_engine->ECS->create_entity();

    FumoRect grav_field_fumo_rect = make_default_field_rect(position);
    fumo_engine->ECS->entity_add_components(
        entity_id,
        ParallelGravityField {
            .field_fumo_rect = grav_field_fumo_rect,
            .gravity_strength = GRAV_STRENGTH,
        });

    fumo_engine->ECS->entity_add_components(entity_id, GravFieldFlag {});
    fumo_engine->ECS->entity_add_components(entity_id,
                                            Render {.color = FUMO_ORANGE});
    fumo_engine->ECS->entity_add_components(
        entity_id,
        Body {.position = {grav_field_fumo_rect.x, grav_field_fumo_rect.y},
              .velocity = {0.0f, 0.0f}});

    fumo_engine->ECS->entity_add_components(entity_id, Serialized {0});
    fumo_engine->ECS->entity_add_components(entity_id, LevelId {0});

    sys_entities.insert(entity_id);

    return entity_id;
}

EntityId LevelEntityFactory::create_rect_field(FumoVec2 position,
                                               FumoVec2 grav_direction) {
    EntityId entity_id = fumo_engine->ECS->create_entity();

    FumoRect grav_field_fumo_rect = make_default_field_rect(position);
    fumo_engine->ECS->entity_add_components(
        entity_id,
        ParallelGravityField {
            .field_fumo_rect = grav_field_fumo_rect,
            .gravity_direction = grav_direction,
            .gravity_strength = GRAV_STRENGTH,
        });

    fumo_engine->ECS->entity_add_components(entity_id, GravFieldFlag {});
    fumo_engine->ECS->entity_add_components(entity_id,
                                            Render {.color = FUMO_ORANGE});
    fumo_engine->ECS->entity_add_components(
        entity_id,
        Body {.position = {grav_field_fumo_rect.x, grav_field_fumo_rect.y},
              .velocity = {0.0f, 0.0f}});

    fumo_engine->ECS->entity_add_components(entity_id, Serialized {0});
    fumo_engine->ECS->entity_add_components(entity_id, LevelId {0});

    sys_entities.insert(entity_id);

    return entity_id;
}

[[nodiscard]] FumoRect make_default_field_rect(FumoVec2 position) {
    return {.x = position.x,
            .y = position.y,
            .width = RECT_WIDTH,
            .height = RECT_HEIGHT};
}

[[nodiscard]] FumoRect make_default_ground_rect(FumoVec2 position) {
    return {.x = position.x,
            .y = position.y,
            .width = RECT_WIDTH,
            .height = RECT_HEIGHT};
}

// TODO: remove this function later
// --------------------------------------------------------------------
EntityId
LevelEntityFactory::debug__internal_create_rect_planet(FumoVec2 position) {

    EntityId entity_id = create_rect(position);
    create_rect_field({.x = position.x, .y = position.y - RECT_HEIGHT});

    return entity_id;
}
