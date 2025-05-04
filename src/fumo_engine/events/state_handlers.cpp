#include "constants/movement_constants.hpp"
#include "fumo_engine/core/fumo_engine.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

void StateHandler::handle_states() {
    // update all states
    for (const auto& entity_id : sys_entities) {
        const auto& entity_state =
            fumo_engine->ECS->get_component<EntityState>(entity_id);
        handle_state(entity_id, entity_state);
    }

    end_of_frame_update();
}

// NOTE: order matters in this function
void StateHandler::handle_state(const EntityId& entity_id,
                                const EntityState& entity_state) {

    auto& player_body = fumo_engine->ECS->get_component<Body>(entity_id);
    auto& player_capsule = fumo_engine->ECS->get_component<Capsule>(entity_id);
    auto& player_animation =
        fumo_engine->ECS->get_component<AnimationInfo>(entity_id);
    auto& player_state = fumo_engine->ECS->get_component<EntityState>(entity_id);
    auto& moved_event_data =
        fumo_engine->ECS->get_component<MovedEventData>(entity_id);

    // screen_transition_handler(player_state);

    // if we face left, then we invert the sprite
    player_body.inverse_direction =
        (moved_event_data.continue_in_direction == DIRECTION::LEFT);

    if (!player_state.colliding) {
        player_state.on_ground = false;
    }

    PRINT_JSON_REPR(player_state);

    // PRINT(player_state.colliding)

    dash_state_handler(player_body,
                       player_capsule,
                       player_animation,
                       player_state);

    if (!player_state.dashing) {
        jump_and_gravity_state_handler(player_body,
                                       player_capsule,
                                       player_animation,
                                       player_state);
    }
    movement_state_handler(player_body, player_capsule, player_state);

    // player_state.falling = !player_state.on_ground && !player_state.colliding;
}

void StateHandler::end_of_frame_update() {
    // auto& player_body = fumo_engine->ECS->get_component<Body>(fumo_engine->player_id);

    // camera follows player

    // if (!player_state.is_changing_screens) {
    //     // freeze the player in place if we change screens
    //     // NOTE: skip over every system other than rendering
    //     // and the event for moving the camera
    //     // (think about how to implement this later)
    //     return;
    // }
}
