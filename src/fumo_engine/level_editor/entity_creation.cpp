#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_engine/level_editor/level_editor.hpp"
#include "objects/factory_systems/factory_systems.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

void DebugLevelEditor::delete_planet(FumoVec2 mouse_position) {
    for (const auto& entity_id : sys_entities) {
        auto& body = fumo_engine->ECS->get_component<Body>(entity_id);
        float distance = FumoVec2Distance(body.position, mouse_position);

        if (distance < mouse_radius) {
            const auto& planet_factory =
                fumo_engine->ECS->get_system<LevelEntityFactory>();
            planet_factory->delete_planet(entity_id);
            return;
        }
    }
}

void DebugLevelEditor::spawn_circular_planet(FumoVec2 mouse_position) {
    const auto& planet_factory =
        fumo_engine->ECS->get_system<LevelEntityFactory>();
    EntityId entity_id = planet_factory->create_circular_planet(mouse_position);
}

void DebugLevelEditor::spawn_circular_field(FumoVec2 mouse_position) {
    const auto& planet_factory =
        fumo_engine->ECS->get_system<LevelEntityFactory>();
    EntityId entity_id = planet_factory->create_circular_field(mouse_position);
}

void DebugLevelEditor::spawn_circle(FumoVec2 mouse_position) {
    const auto& planet_factory =
        fumo_engine->ECS->get_system<LevelEntityFactory>();
    EntityId entity_id = planet_factory->create_circle(mouse_position);
}

void DebugLevelEditor::spawn_transition_line(FumoVec2 mouse_position) {
    const auto& planet_factory =
        fumo_engine->ECS->get_system<LevelEntityFactory>();
    planet_factory->create_screen_transition(mouse_position);
}

void DebugLevelEditor::spawn_rect(FumoVec2 mouse_position) {
    const auto& planet_factory =
        fumo_engine->ECS->get_system<LevelEntityFactory>();
    planet_factory->create_rect(mouse_position);
}

void DebugLevelEditor::spawn_rect_field(FumoVec2 mouse_position) {
    const auto& planet_factory =
        fumo_engine->ECS->get_system<LevelEntityFactory>();
    planet_factory->create_rect_field(mouse_position);
}
