#include "constants/movement_constants.hpp"
#include "fumo_engine/core/fumo_engine.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;

void StateHandler::jump_and_gravity_state_handler(
    Body& player_body,
    Capsule& player_capsule,
    AnimationInfo& player_animation,
    EntityState& player_state) {

    const auto& gravity_direction = player_body.real_gravity_direction;
    const auto& gravity_strength = player_body.current_gravity_strength;

    float factor = 6;
    float jump_progress; // curently unused
    if (player_state.jumping) {

        if (player_animation.frame_progress
            != player_animation.sprite_frame_count) {
            AnimationPlayer::play(player_animation, "jump");
        }

        float jump_amount =
            FumoVec2DotProduct(player_body.get_real_y_velocity(),
                               gravity_direction);

        bool going_up = jump_amount < 0;

        if (going_up) jump_progress = 1 + jump_amount / JUMP_SCALING;
        else {
            jump_progress = jump_amount / JUMP_SPEED_CAP;
            factor += 4; // use jump_progress for easing if desired
        }

        if (IsKeyReleased(KEY_SPACE)) {
            player_body.velocity = player_body.get_real_x_velocity()
                + player_body.get_real_y_velocity() * 0.5f;
        }

        if (!IsKeyDown(KEY_SPACE)) factor += 2;
    }

    // apply gravity changes
    FumoVec2 acceleration = gravity_direction * gravity_strength * factor;
    player_body.velocity += acceleration * fumo_engine->frametime;

    // update animation when nothing is happening so it looks nicer
    if (!player_state.idle) {
        if (player_animation.current_sheet_name != "dash") {
            AnimationPlayer::play(player_animation, "jump");
            player_animation.frame_progress = 3;
            player_animation.current_region_rect.x =
                player_animation.current_region_rect.width
                * player_animation.frame_progress;
            player_animation.is_running = true;
        }
    }
}
