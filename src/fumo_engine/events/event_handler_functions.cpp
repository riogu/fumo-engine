#include "fumo_engine/core/global_state.hpp"
#include "fumo_engine/events/event_state_handlers.hpp"
#include "objects/generic_systems/systems.hpp"

extern std::unique_ptr<GlobalState> global;

namespace EventHandler {

void jumped(const Event& event) {
    auto& player_body = global->ECS->get_component<Body>(event.entity_id);
    auto& player_state = global->ECS->get_component<EntityState>(event.entity_id);
    if (!player_state.can_jump) {
        return;
    }

    if (player_state.jumping) {
        // cancel previous jump if we were in the middle of it
        player_body.iterations = 0;
    }

    if (player_state.on_ground) {
        player_state.jumping = true;
        player_state.on_ground = false;
        player_state.can_jump = false;
        BodyMovement::jump(player_body, event.entity_id);
    }
}

void idle(const Event& event) {
    auto& player_animation =
        global->ECS->get_component<AnimationInfo>(event.entity_id);
    auto& player_state = global->ECS->get_component<EntityState>(event.entity_id);

    if (player_state.on_ground) {
        AnimationPlayer::play(player_animation, "idle");
    }

    // reset moved_wrapper so we can allow for smoother controls,
    // whilc also not breaking the direction of the movement when we stop
    EntityQuery query_moved_event {
        .component_mask = global->ECS->make_component_mask<MovedEventData>(),
        .component_filter = Filter::All};

    if (global->ECS->filter(event.entity_id, query_moved_event)) {
        auto& moved_event_data =
            global->ECS->get_component<MovedEventData>(event.entity_id);
        moved_event_data.previous_direction = DIRECTION::NO_DIRECTION;
    }
}

void swapped_orbits(const Event& event) {
    auto& player_state = global->ECS->get_component<EntityState>(event.entity_id);

    player_state.can_swap_orbits = false;
}

void collided(const Event& event) {
    auto& player_state = global->ECS->get_component<EntityState>(event.entity_id);

    player_state.on_ground = true;
    player_state.can_swap_orbits = true;
    player_state.can_jump = true;
}

} // namespace EventHandler

// want this behavior:
// > when the player presses a direction, we check what that direction
// > is relative to the gravity direction coordinates
// > then we solve as follows:
// -> we take inputs literally, as the player requesting to move towards a direction.
// -> then we check if that is possible based on rotation angle
// -> we either accept, or reject the input

enum struct ACCEPT { HORIZONTAL, VERTICAL, HORIZONTAL_INVERT, VERTICAL_INVERT };

DIRECTION opposite_direction(DIRECTION direction) {
    if (direction == DIRECTION::LEFT) {
        return DIRECTION::RIGHT;
    }
    if (direction == DIRECTION::RIGHT) {
        return DIRECTION::LEFT;
    }
    if (direction == DIRECTION::UP) {
        return DIRECTION::DOWN;
    }
    if (direction == DIRECTION::DOWN) {
        return DIRECTION::UP;
    }
    PANIC("UNREACHABLE OR GAVE PREVIOUS_DIRECTION (FIX THIS)");
    std::unreachable();
}

// ACCEPT find_acceptance_state(float rotation) {
//     PRINT(rotation)
//     if (-135 <= rotation && rotation <= -45) {
//         return ACCEPT::VERTICAL_INVERT;
//     }
//     if (45 <= rotation && rotation <= 135) {
//         return ACCEPT::VERTICAL;
//     }
//     if (-45 <= rotation && rotation <= 45) {
//         return ACCEPT::HORIZONTAL;
//     }
//     if (135 <= rotation || rotation <= -135) {
//         return ACCEPT::HORIZONTAL_INVERT;
//     }
//     std::unreachable();
// }
ACCEPT find_acceptance_state(float rotation) {
    PRINT(rotation)
    if (-135 <= rotation && rotation <= -45) {
        return ACCEPT::VERTICAL_INVERT;
    }
    if (-180 <= rotation && rotation <= 0) {
        return ACCEPT::VERTICAL;
    }
    if (-45 <= rotation && rotation <= 45) {
        return ACCEPT::HORIZONTAL;
    }
    if (135 <= rotation || rotation <= -135) {
        return ACCEPT::HORIZONTAL_INVERT;
    }
    std::unreachable();
}

[[nodiscard]] Vector2 direction_to_vector(DIRECTION direction) {
    switch (direction) {
        case DIRECTION::LEFT:
            return {-1.0f, 0.0f};
        case DIRECTION::RIGHT:
            return {1.0f, 0.0f};
        case DIRECTION::UP:
            return {0.0f, -1.0f};
        case DIRECTION::DOWN:
            return {0.0f, 1.0f};
        default:
            PRINT("sent to unreachable code NO_DIRECTION");
            std::unreachable();
            break;
    }
}

DIRECTION find_real_direction(DIRECTION direction, const Body& body) {
    Vector2 vec_direction = direction_to_vector(direction);
    return (Vector2DotProduct(vec_direction, body.x_direction) > 0) ? DIRECTION::RIGHT
                                                                    : DIRECTION::LEFT;
}

void MovedWrapper::moved_event() {
    auto& player_body = global->ECS->get_component<Body>(entity_id);
    auto& player_state = global->ECS->get_component<EntityState>(entity_id);
    auto& move_event_data = global->ECS->get_component<MovedEventData>(entity_id);

    const auto& grav_direction = player_body.gravity_direction;
    const auto& rotation = player_body.rotation;

    DIRECTION& previous_direction = move_event_data.previous_direction;
    DIRECTION& direction = move_event_data.direction;
    DIRECTION& continue_in_direction = move_event_data.continue_in_direction;

    //--------------------------------------------------------------------------------------
    // previous frame checking (for more responsive player controller)
    PRINT(previous_direction);
    PRINT(direction);

    if (previous_direction == direction) {
        // allow player to hold a key and continue moving along the planet
        BodyMovement::move(entity_id, continue_in_direction);
        return;

    } else if (previous_direction == opposite_direction(direction)) {
        // also allow them to switch to the other key too without losing control
        continue_in_direction = opposite_direction(continue_in_direction);
        previous_direction = direction;
        BodyMovement::move(entity_id, continue_in_direction);
        return;
    }
    //--------------------------------------------------------------------------------------

    DIRECTION real_direction = find_real_direction(direction, player_body);
    continue_in_direction = real_direction;
    previous_direction = direction;
    BodyMovement::move(entity_id, real_direction);
}
