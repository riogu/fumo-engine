#pragma once
#include "fumo_engine/core/system_base.hpp"
#include "fumo_raylib.hpp"

class LevelEntityFactory: public System {
    // NOTE: planet factory now keeps track of the entities
    // it created so that we can delete only the ones
    // created by it (so we can undo input handler actions without altering
    // the rest of the planets already on screen)
  public:
    void sys_call() override {};

    void delete_planet(EntityId entity_id);
    void delete_all_planets();

    EntityId create_circular_planet(FumoVec2 position);
    EntityId create_circular_field(FumoVec2 position);
    EntityId create_circle(FumoVec2 position);

    EntityId create_rect(FumoVec2 position);

    EntityId create_outline_rect(FumoVec2 position);
    EntityId create_outline_rect(FumoRect rect);
    EntityId create_rect_field(FumoVec2 position);
    EntityId create_rect_field(FumoVec2 position, FumoVec2 grav_direction);

    //remove later (used for hardcoded level0)
    EntityId debug__internal_create_rect_planet(FumoVec2 position);

    EntityId create_screen_transition(FumoVec2 position);
};
