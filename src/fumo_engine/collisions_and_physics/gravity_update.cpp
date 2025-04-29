#include "fumo_engine/collisions_and_physics/gravity_field_systems.hpp"
#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_raylib.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

void GravityUpdater::gravity_update() {
    auto& player_body =
        fumo_engine->ECS->get_component<Body>(fumo_engine->player_id);
    update_gravity(fumo_engine->player_id, player_body);
}

void GravityUpdater::update_gravity(EntityId entity_id, Body& player_body) {
    // NOTE: points towards the planet's planet_body
    auto& player_state =
        fumo_engine->ECS->get_component<EntityState>(entity_id);

    if (player_state.player_owning_field == NO_ENTITY_FOUND
        || player_state.player_owning_field == 0) {
        return;
    }

    float gravity_strength {};

    EntityId planet_id = player_state.player_owning_field;
    EntityQuery query_parallel {
        .component_mask =
            fumo_engine->ECS->make_component_mask<ParallelGravityField>(),
        .component_filter = Filter::All};

    const auto& body_planet = fumo_engine->ECS->get_component<Body>(planet_id);
    FumoVec2 gravity_direction {};

    // --------------------------------------------------------------------
    // mario galaxy-like changing of the left-right movement to match
    // the player expectation of what right and left should be
    EntityQuery skip_parallel {
        .component_mask =
            fumo_engine->ECS->make_component_mask<ParallelGravityField>(),
        .component_filter = Filter::None};

    // --------------------------------------------------------------------

    if (fumo_engine->ECS->filter(planet_id, query_parallel)) {
        // methods for parallel gravity fields
        auto& parallel_field =
            fumo_engine->ECS->get_component<ParallelGravityField>(planet_id);
        parallel_field.update_gravity(player_body);

        gravity_direction = parallel_field.gravity_direction;
        gravity_strength = parallel_field.gravity_strength;
    } else {
        // methods for circular gravity fields
        auto& circular_field =
            fumo_engine->ECS->get_component<CircularGravityField>(planet_id);

        circular_field.update_gravity(player_body, body_planet);

        gravity_direction = circular_field.gravity_direction;
        gravity_strength = circular_field.gravity_strength;
    }

    // --------------------------------------------------------------------
    // update body with new gravity information
    player_body.current_gravity_strength = gravity_strength;
    player_body.real_gravity_direction = gravity_direction;
    player_body.real_x_direction = {player_body.real_gravity_direction.y,
                                    -player_body.real_gravity_direction.x};
    player_body.rotation = std::atan2(player_body.real_x_direction.y,
                                      player_body.real_x_direction.x)
        * RAD2DEG;
    // --------------------------------------------------------------------
    // update peidro's dash stuff
    // (for some reason this update needs to happen here)
    player_body.dash_gravity_direction = player_state.dashing
        ? FumoVec2 {player_body.dash_x_direction.y,
                    -player_body.dash_x_direction.x}
        : player_body.real_gravity_direction;
}

void ParallelGravityField::update_gravity(Body& player_body) {
    // do nothing, parallel fields stay the same unless they rotate
    // TODO: implement updating based on rotation in this function
    //
}

void CircularGravityField::update_gravity(Body& player_body,
                                          const Body& body_planet) {
    gravity_direction =
        FumoVec2Normalize(body_planet.position - player_body.position);
}

// player_body.real_gravity_direction = gravity_direction;
// player_body.real_x_direction = {player_body.real_gravity_direction.y,
//                                 -player_body.real_gravity_direction.x};
//
// player_body.rotation = std::atan2(player_body.real_x_direction.y,
//                                   player_body.real_x_direction.x)
//     * RAD2DEG;
//
// if (player_state.dashing) {
//     player_body.dash_gravity_direction = {player_body.dash_x_direction.y,
//                                           -player_body.dash_x_direction.x};
//     return;
// }
// player_body.dash_gravity_direction = gravity_direction;
//
// float factor = 6;
// if (player_state.jumping) {
//     float jump_amount =
//         FumoVec2DotProduct(player_body.get_real_y_velocity(),
//                            gravity_direction);
//     bool going_up = jump_amount < 0;
//
//     if (going_up) {
//         float jump_progress = 1 + jump_amount / jump_scaling;
//         // PRINT("jumping up");
//         // PRINT(jump_progress);
//     } else {
//         float jump_progress = jump_amount / jump_speed_cap;
//         // PRINT("jumping down");
//         // PRINT(jump_progress);
//
//         factor += 4; // use jump_progress for easing if desired
//     }
//
//     if (IsKeyReleased(KEY_SPACE)) {
//
//         player_body.velocity = player_body.get_real_x_velocity()
//             + player_body.get_real_y_velocity() * 0.5f;
//     }
//
//     if (!IsKeyDown(KEY_SPACE)) {
//         factor += 2;
//     }
// }
//
// FumoVec2 acceleration = gravity_direction * gravity_strength * factor;
//
// player_body.velocity += acceleration * fumo_engine->frametime;
