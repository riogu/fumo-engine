#include "fumo_engine/collisions_and_physics/collision_runner.hpp"

#include "fumo_engine/collisions_and_physics/collision_functions.hpp"
#include "fumo_engine/core/fumo_engine.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

void CollisionRunner::check_collisions() {
    // TODO: change the collisions to use raycasting (so they work very consistently)
    // (do this later once its finished since it is good enough for now)
    Collision collision {};
    CollisionEventData& collision_event =
        fumo_engine->ECS->get_component<CollisionEventData>(
            fumo_engine->player_id);

    bool collision_happened = false;
    constexpr float substep_count = 10;

    const auto& player_id = fumo_engine->player_id;
    auto& player_body =
        fumo_engine->ECS->get_component<Body>(fumo_engine->player_id);
    auto& player_capsule =
        fumo_engine->ECS->get_component<Capsule>(fumo_engine->player_id);
    auto& player_state =
        fumo_engine->ECS->get_component<EntityState>(fumo_engine->player_id);

    const EntityQuery fumo_rect_query {
        .component_mask = fumo_engine->ECS->make_component_mask<FumoRect>(),
        .component_filter = Filter::All};

    const EntityQuery circle_query {
        .component_mask = fumo_engine->ECS->make_component_mask<FumoRect>(),
        .component_filter = Filter::All};

    for (const auto& obstacle_id : sys_entities) {
        const auto& obstacle_body =
            fumo_engine->ECS->get_component<Body>(obstacle_id);

        if (fumo_engine->ECS->filter(obstacle_id, fumo_rect_query)) {
            const auto& fumo_rect =
                fumo_engine->ECS->get_component<FumoRect>(obstacle_id);
            collision =
                Collisions::continuous_rect_collision_solving(player_capsule,
                                                              player_body,
                                                              fumo_rect,
                                                              obstacle_body,
                                                              substep_count);
            if (collision.collided) {
                collision_happened = true;
                collision_event.all_collisions.push_back(collision);
                // print_direction(collision.collided_capsule_side);
            }
            continue;
        }

        const auto& circle_shape =
            fumo_engine->ECS->get_component<Circle>(obstacle_id);
        collision =
            Collisions::capsule_to_circle_collision_solving(player_capsule,
                                                            player_body,
                                                            circle_shape,
                                                            obstacle_body,
                                                            substep_count);
        if (collision.collided) {
            collision_happened = true;
            collision_event.all_collisions.push_back(collision);
            // print_direction(collision.collided_capsule_side);
        }
        continue;
    }

    player_state.colliding = collision_happened;

    if (collision_happened) {
        // PRINT(collision_event.all_collisions.size())
        fumo_engine->event_handler->add_event(
            {.event = EVENT_::ENTITY_COLLIDED,
             .entity_id = fumo_engine->player_id});
    }
}

#define CHECK_COLLISION(_SHAPE, query, function_name) \
    if (fumo_engine->ECS->filter(obstacle_id, query)) { \
        Collision collision = Collisions::function_name( \
            line, \
            fumo_engine->ECS->get_component<_SHAPE>(obstacle_id), \
            fumo_engine->ECS->get_component<Body>(obstacle_id)); \
        if (collision.collided \
            && (collision.distance < closest_collision.distance \
                || closest_collision.distance == 0.0f)) { \
            closest_collision = collision; \
        } \
        continue; \
    }

[[nodiscard]] const Collision
CollisionRunner::check_raycast_line(const Line& line) {

    Collision closest_collision;

    const EntityQuery fumo_rect_query {
        .component_mask = fumo_engine->ECS->make_component_mask<FumoRect>(),
        .component_filter = Filter::All};

    const EntityQuery circle_query {
        .component_mask = fumo_engine->ECS->make_component_mask<Circle>(),
        .component_filter = Filter::All};

    for (const auto& obstacle_id : sys_entities) {
        CHECK_COLLISION(FumoRect, fumo_rect_query, LineToRectCollision);
        CHECK_COLLISION(Circle, circle_query, LineToCircleCollision);
    }

    return closest_collision;
}
