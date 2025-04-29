#include "constants/planet_constants.hpp"
#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_engine/screen_components.hpp"
#include "objects/factory_systems/factory_systems.hpp"
extern std::unique_ptr<FumoEngine> fumo_engine;

EntityId LevelEntityFactory::create_circular_planet(FumoVec2 position) {

    EntityId entity_id = fumo_engine->ECS->create_entity();

    fumo_engine->ECS->entity_add_component(entity_id, ColliderObjectFlag {});
    fumo_engine->ECS->entity_add_component(entity_id, GravFieldFlag {});
    fumo_engine->ECS->entity_add_component(
        entity_id,
        Body {.position = position, .velocity = {0.0f, 0.0f}});
    fumo_engine->ECS->entity_add_component(entity_id,
                                           Render {.color = FUMO_BLUE});
    fumo_engine->ECS->entity_add_component(
        entity_id,
        Circle {.radius = CIRCLE_RADIUS * 4});

    fumo_engine->ECS->entity_add_component(entity_id, Screen {0});
    fumo_engine->ECS->entity_add_component(entity_id, LevelId {1});

    fumo_engine->ECS->entity_add_component(
        entity_id,
        CircularGravityField {.radius = GRAV_REACH * 3,
                              .gravity_strength = GRAV_STRENGTH,
                              .position = position});

    sys_entities.insert(entity_id);

    return entity_id;
}

EntityId LevelEntityFactory::create_circular_field(FumoVec2 position) {

    EntityId entity_id = fumo_engine->ECS->create_entity();

    fumo_engine->ECS->entity_add_component(
        entity_id,
        CircularGravityField {.radius = GRAV_REACH * 3,
                              .gravity_strength = GRAV_STRENGTH,
                              .position = position});

    fumo_engine->ECS->entity_add_component(entity_id, GravFieldFlag {});

    fumo_engine->ECS->entity_add_component(
        entity_id,
        Body {.position = position, .velocity = {0.0f, 0.0f}});

    fumo_engine->ECS->entity_add_component(entity_id,
                                           Render {.color = FUMO_BLUE});

    fumo_engine->ECS->entity_add_component(entity_id, Screen {0});
    fumo_engine->ECS->entity_add_component(entity_id, LevelId {1});

    sys_entities.insert(entity_id);
    return entity_id;
}

EntityId LevelEntityFactory::create_circle(FumoVec2 position) {

    EntityId entity_id = fumo_engine->ECS->create_entity();

    fumo_engine->ECS->entity_add_component(entity_id, ColliderObjectFlag {});
    fumo_engine->ECS->entity_add_component(
        entity_id,
        Body {.position = position, .velocity = {0.0f, 0.0f}});
    fumo_engine->ECS->entity_add_component(entity_id,
                                           Render {.color = FUMO_BLUE});
    fumo_engine->ECS->entity_add_component(
        entity_id,
        Circle {.radius = CIRCLE_RADIUS * 4});

    fumo_engine->ECS->entity_add_component(entity_id, Screen {0});
    fumo_engine->ECS->entity_add_component(entity_id, LevelId {1});

    sys_entities.insert(entity_id);

    return entity_id;
}
