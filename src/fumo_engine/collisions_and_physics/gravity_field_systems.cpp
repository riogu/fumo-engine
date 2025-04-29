#include "fumo_engine/collisions_and_physics/gravity_field_systems.hpp"

#include "fumo_engine/collisions_and_physics/collision_functions.hpp"
#include "fumo_engine/core/fumo_engine.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

// RULES:
// - give priority to field change
// - don't change fields if the player is on the ground
// - only allow picking candidates again if the player touches the ground
void GravityFieldHandler::find_gravity_field() {
    auto& player_state =
        fumo_engine->ECS->get_component<EntityState>(fumo_engine->player_id);

    if (!player_state.can_swap_orbits || player_state.on_ground) {
        return;
    }

    std::vector<EntityId> candidate_planets;
    candidate_planets.reserve(10);

    const auto& player_id = fumo_engine->player_id;
    auto& player_body =
        fumo_engine->ECS->get_component<Body>(fumo_engine->player_id);
    auto& player_capsule =
        fumo_engine->ECS->get_component<Capsule>(fumo_engine->player_id);

    EntityQuery query_parallel {
        .component_mask =
            fumo_engine->ECS->make_component_mask<ParallelGravityField>(),
        .component_filter = Filter::All};

    for (const auto& planet_id : sys_entities) {
        const auto& body_planet =
            fumo_engine->ECS->get_component<Body>(planet_id);

        if (fumo_engine->ECS->filter(planet_id, query_parallel)) {
            // methods for parallel gravity fields
            const auto& parallel_field =
                fumo_engine->ECS->get_component<ParallelGravityField>(
                    planet_id);
            if (parallel_field.is_inside_field(player_body,
                                               player_capsule,
                                               body_planet)) {
                candidate_planets.push_back(planet_id);
            }
            continue;
        }
        // methods for circular gravity fields
        const auto& circular_field =
            fumo_engine->ECS->get_component<CircularGravityField>(planet_id);

        if (circular_field.is_inside_field(player_body,
                                           player_capsule,
                                           body_planet)) {
            candidate_planets.push_back(planet_id);
        }
    }

    // -------------------------------------------------------------------------------
    // pick the final field
    // -------------------------------------------------------------------------------
    // NOTE:
    // - we can (hopefully) assume the player is at most inside 2 fields!!!
    // dont add more than 2 fields, if you do, change the code to support that

    for (const auto& planet_id : candidate_planets) {
        if (planet_id != player_state.player_owning_field) {
            player_state.player_owning_field = planet_id;

            fumo_engine->event_handler->add_event(
                {.event = EVENT_::ENTITY_SWAPPED_ORBITS,
                 .entity_id = player_id});

            return;
        }
    }
}

bool ParallelGravityField::is_inside_field(const Body& player_body,
                                           const Capsule& player_capsule,
                                           const Body& parallel_body) const {
    Collision collision = Collisions::CapsuleToRectCollision(player_capsule,
                                                             player_body,
                                                             field_fumo_rect,
                                                             parallel_body);
    // Body {.position = {field_fumo_rect.x, field_fumo_rect.y}});

    // if overlap == 0, then there was no collision
    return (collision.overlap != 0);
}

bool CircularGravityField::is_inside_field(const Body& player_body,
                                           const Capsule& player_capsule,
                                           const Body& circular_body) const {
    // -------------------------------------------------------------------------------
    // check for collisions with capsule
    // -------------------------------------------------------------------------------
    float radius_sum = player_capsule.radius + radius;

    // -------------------------------------------------------------------------------
    // top circle collision check
    float top_distance = FumoVec2Distance(circular_body.position,
                                          player_capsule.top_circle_center);
    if (top_distance < radius_sum) {
        // collided with top circle
        return true;
    }
    // -------------------------------------------------------------------------------
    // bottom circle collision check
    float bottom_distance =
        FumoVec2Distance(circular_body.position,
                         player_capsule.bottom_circle_center);
    if (bottom_distance < radius_sum) {
        // collided with top circle
        return true;
    }

    // -------------------------------------------------------------------------------
    // TODO: remove the side checks later
    //
    // capsule sides collision check
    // const auto left_line_distance_pair = PointToLineDistanceAndIntersection(
    //     circular_body.position, player_capsule.left_line);
    //
    // if (left_line_distance_pair.first != 0 &&
    //     left_line_distance_pair.first < gravity_radius) {
    //     // collided with left_line
    //     return true;
    // }
    //
    // const auto right_line_distance_pair = PointToLineDistanceAndIntersection(
    //     circular_body.position, player_capsule.right_line);
    //
    // if (right_line_distance_pair.first != 0 &&
    //     right_line_distance_pair.first < gravity_radius) {
    //     // collided with right_line
    //     return true;
    // }
    // -------------------------------------------------------------------------------
    return false;
}
