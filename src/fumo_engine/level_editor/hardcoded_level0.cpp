#include "constants/fumo_raylib_constants.hpp"
#include "constants/planet_constants.hpp"
#include "fumo_engine/core/fumo_engine.hpp"
#include "objects/factory_systems/factory_systems.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

FumoVec2 starter_position = {0.0f, screenCenter.y + 400};
float previous_width = 500.0f;

FumoVec2 screenborders_position = {0.0f, 0.0f};
uint64_t curr_id = 0;

void make_some_rects();
void screen_border_lines();

void debug_spawn_level_objects() {
    // make_some_rects();
    // screen_border_lines();
    const auto& planet_factory =
        fumo_engine->ECS->get_system<LevelEntityFactory>();

    {
        // make first planet that owns the player
        EntityId id_planet = planet_factory->debug__internal_create_rect_planet(
            {starter_position.x, starter_position.y});

        auto& player_state = fumo_engine->ECS->get_component<EntityState>(
            fumo_engine->player_id);
        player_state.player_owning_field = id_planet + 1;

        FumoRect& rect_planet =
            fumo_engine->ECS->get_component<FumoRect>(id_planet);
        rect_planet.width = screenWidth;
        previous_width = rect_planet.width;

        ParallelGravityField& rect_field =
            fumo_engine->ECS->get_component<ParallelGravityField>(id_planet
                                                                  + 1);
        rect_field.field_fumo_rect.width = rect_planet.width;
    }
    {
        EntityId id_planet =
            planet_factory->create_outline_rect(FumoVec2 {0.0f, 0.0f});
        fumo_engine->ECS->get_component<Body>(id_planet).position =
            screenborders_position;
        fumo_engine->ECS->entity_add_components(
            fumo_engine->ECS->create_entity(),
            Screen {.screen_id = curr_id++,
                    .screen_position = screenborders_position + screenCenter},
            Serialized {.file_id = 0},
            LevelId {.level_id = 0});
    }
    {
        EntityId id_planet =
            planet_factory->create_outline_rect(FumoVec2 {0.0f, 0.0f});
        screenborders_position.x += screenWidth;
        fumo_engine->ECS->get_component<Body>(id_planet).position =
            screenborders_position;
        fumo_engine->ECS->entity_add_components(
            fumo_engine->ECS->create_entity(),
            Screen {.screen_id = curr_id++,
                    .screen_position = screenborders_position + screenCenter},
            Serialized {.file_id = 0},
            LevelId {.level_id = 0});
    }
    {
        EntityId id_planet =
            planet_factory->create_outline_rect(FumoVec2 {0.0f, 0.0f});
        screenborders_position.x += screenWidth;

        fumo_engine->ECS->get_component<Body>(id_planet).position =
            screenborders_position;

        fumo_engine->ECS->entity_add_components(
            fumo_engine->ECS->create_entity(),
            Screen {.screen_id = curr_id++,
                    .screen_position = screenborders_position + screenCenter},
            Serialized {.file_id = 0},
            LevelId {.level_id = 0});
    }
    {
        EntityId id_planet =
            planet_factory->create_outline_rect(FumoVec2 {0.0f, 0.0f});
        screenborders_position.x += screenWidth;
        fumo_engine->ECS->get_component<Body>(id_planet).position =
            screenborders_position;
        fumo_engine->ECS->entity_add_components(
            fumo_engine->ECS->create_entity(),
            Screen {.screen_id = curr_id++,
                    .screen_position = screenborders_position + screenCenter},
            Serialized {.file_id = 0},
            LevelId {.level_id = 0});
    }
    {
        EntityId id_planet =
            planet_factory->create_outline_rect(FumoVec2 {0.0f, 0.0f});
        screenborders_position.x += screenWidth;
        fumo_engine->ECS->get_component<Body>(id_planet).position =
            screenborders_position;
        fumo_engine->ECS->entity_add_components(
            fumo_engine->ECS->create_entity(),
            Screen {.screen_id = curr_id++,
                    .screen_position = screenborders_position + screenCenter},
            Serialized {.file_id = 0},
            LevelId {.level_id = 0});
    }
}

void screen_border_lines() {
    const auto& planet_factory =
        fumo_engine->ECS->get_system<LevelEntityFactory>();

    {
        EntityId id_planet = planet_factory->create_rect(starter_position);
        fumo_engine->ECS->entity_add_components(id_planet, OutlineRect {});

        FumoRect& outline_rect =
            fumo_engine->ECS->get_component<FumoRect>(id_planet);
        outline_rect = {.x = 0.0f,
                        .y = 0.0f,
                        .width = screenWidth,
                        .height = screenHeight};
    }
    {
        EntityId id_planet = planet_factory->create_rect(starter_position);
        fumo_engine->ECS->entity_add_components(id_planet, OutlineRect {});

        FumoRect& outline_rect =
            fumo_engine->ECS->get_component<FumoRect>(id_planet);
        outline_rect = {.x = screenWidth,
                        .y = 0.0f,
                        .width = screenWidth,
                        .height = screenHeight};
    }
    {
        EntityId id_planet = planet_factory->create_rect(starter_position);
        fumo_engine->ECS->entity_add_components(id_planet, OutlineRect {});

        FumoRect& outline_rect =
            fumo_engine->ECS->get_component<FumoRect>(id_planet);
        outline_rect = {.x = screenWidth * 2,
                        .y = 0.0f,
                        .width = screenWidth,
                        .height = screenHeight};
    }
    {
        EntityId id_planet = planet_factory->create_rect(starter_position);
        fumo_engine->ECS->entity_add_components(id_planet, OutlineRect {});

        FumoRect& outline_rect =
            fumo_engine->ECS->get_component<FumoRect>(id_planet);
        outline_rect = {.x = screenWidth * 3,
                        .y = 0.0f,
                        .width = screenWidth,
                        .height = screenHeight};
    }
    FumoVec2 positon = screenCenter;
    positon.y -= 230;
    {
        EntityId circle_planet =
            planet_factory->create_circular_planet(positon);
    }
    positon.x += 900;
    {
        EntityId circle_planet =
            planet_factory->create_circular_planet(positon);
        EntityId circle_planet2 = planet_factory->create_circular_planet(
            {positon.x, positon.y - 900});
    }
    positon.x += 900;
    {
        EntityId circle_planet =
            planet_factory->create_circular_planet(positon);
        EntityId circle_planet2 = planet_factory->create_circular_planet(
            {positon.x, positon.y - 900});
    }
    positon.x += 900;
    {
        EntityId circle_planet =
            planet_factory->create_circular_planet(positon);
        EntityId circle_planet2 = planet_factory->create_circular_planet(
            {positon.x, positon.y - 900});
    }
    positon.x += 900;
    {
        EntityId circle_planet =
            planet_factory->create_circular_planet(positon);
        EntityId circle_planet2 = planet_factory->create_circular_planet(
            {positon.x, positon.y - 900});
    }
    positon.x += 900;
    {
        EntityId circle_planet =
            planet_factory->create_circular_planet(positon);
        EntityId circle_planet2 = planet_factory->create_circular_planet(
            {positon.x, positon.y - 900});
    }
    positon.x += 900;
    {
        EntityId circle_planet =
            planet_factory->create_circular_planet(positon);
        EntityId circle_planet2 = planet_factory->create_circular_planet(
            {positon.x, positon.y - 900});
    }
    positon.x += 900;
    {
        EntityId circle_planet =
            planet_factory->create_circular_planet(positon);
        EntityId circle_planet2 = planet_factory->create_circular_planet(
            {positon.x, positon.y - 900});
    }
    positon.x += 900;
    {
        EntityId circle_planet =
            planet_factory->create_circular_planet(positon);
        EntityId circle_planet2 = planet_factory->create_circular_planet(
            {positon.x, positon.y - 900});
    }
    positon.x += 900;
    {
        EntityId circle_planet =
            planet_factory->create_circular_planet(positon);
        EntityId circle_planet2 = planet_factory->create_circular_planet(
            {positon.x, positon.y - 900});
    }
    FumoVec2 screncentr = screenCenter;
    screncentr.x -= screenWidth / 3.0f;
    {
        EntityId rect_planet =
            planet_factory->debug__internal_create_rect_planet(screncentr);
    }
    screncentr.x -= screenWidth / 3.0f;
    {
        EntityId rect_planet =
            planet_factory->debug__internal_create_rect_planet(screncentr);
    }
    screncentr.x -= screenWidth / 3.0f;
    {
        EntityId rect_planet =
            planet_factory->debug__internal_create_rect_planet(screncentr);
    }
    screncentr.x -= screenWidth / 3.0f;
    {
        EntityId rect_planet =
            planet_factory->debug__internal_create_rect_planet(screncentr);
    }
    screncentr.x -= screenWidth / 3.0f;
    {
        EntityId rect_planet =
            planet_factory->debug__internal_create_rect_planet(screncentr);
    }
    screncentr.x -= screenWidth / 3.0f;
    {
        EntityId rect_planet =
            planet_factory->debug__internal_create_rect_planet(screncentr);
    }
    screncentr.x -= screenWidth / 3.0f;
    {
        EntityId rect_planet =
            planet_factory->debug__internal_create_rect_planet(screncentr);
    }
}

void make_some_rects() {
    const auto& planet_factory =
        fumo_engine->ECS->get_system<LevelEntityFactory>();

    {
        // make first planet that owns the player
        EntityId id_planet = planet_factory->create_rect(
            {starter_position.x - screenWidth * 3, starter_position.y});

        EntityId field_id = planet_factory->create_rect_field(
            {starter_position.x - screenWidth * 3,
             starter_position.y - RECT_HEIGHT});
        auto& player_state = fumo_engine->ECS->get_component<EntityState>(
            fumo_engine->player_id);
        player_state.player_owning_field = field_id;

        FumoRect& rect_planet =
            fumo_engine->ECS->get_component<FumoRect>(id_planet);
        rect_planet.width = rect_planet.width * 100;
        previous_width = rect_planet.width;

        ParallelGravityField& rect_field =
            fumo_engine->ECS->get_component<ParallelGravityField>(field_id);
        rect_field.field_fumo_rect.width = rect_planet.width;
    }

    {
        starter_position.x += previous_width;

        EntityId id_planet = planet_factory->create_rect(starter_position);
        FumoRect& no_grav_rect =
            fumo_engine->ECS->get_component<FumoRect>(id_planet);

        previous_width = no_grav_rect.width;
    }
}
