#include "constants/movement_constants.hpp"
#include "fumo_engine/collisions_and_physics/collision_runner.hpp"
#include "fumo_engine/core/fumo_engine.hpp"
#include "main_functions.hpp"
extern std::unique_ptr<FumoEngine> fumo_engine;

void raycast_jumping(Body& player_body,
                     Capsule& player_capsule,
                     EntityState& player_state) {

    FumoVec2 new_velocity = player_body.velocity;

    Capsule new_capsule = player_capsule;
    new_capsule.update_capsule_positions(player_body);
    const FumoVec2& previous_pos = new_capsule.top_circle_center;

    FumoVec2 new_pos = previous_pos + new_velocity * fumo_engine->frametime;

    const auto& collision_runner =
        fumo_engine->ECS->get_system<CollisionRunner>();
    const Collision& collision = collision_runner->check_raycast_line(
        {.start = previous_pos, .end = new_pos});

    // PRINT(collision.collided)
    // BeginMode2D(*fumo_engine->camera);
    // FumoDrawLineEx(previous_pos, new_pos, 20.0f, FUMO_GOLD);
    // FumoDrawCircleV(collision.intersection_point, 200.0f, FUMO_GREEN);

    if (collision.collided) {

        new_velocity -= collision.normal
            * FumoVec2DotProduct(new_velocity, collision.normal);
        if (player_state.colliding_with_corner) {
            new_velocity /= 2.0f;
        }
    }

    player_body.velocity = new_velocity;

    // FumoDrawLineEx(previous_pos,
    //                previous_pos + player_body.velocity,
    //                50.0f,
    //                FUMO_PINK);
    // EndMode2D();
}

void StateHandler::movement_state_handler(Body& player_body,
                                          Capsule& player_capsule,
                                          EntityState& player_state) {

    //-----------------------------------------------------------------
    // apply movement changes to the player
    if (player_state.on_ground) {
        // dont move in the GRAVITY direction while player is on the ground
        // nudge player SLIGHTLY towards planet to correct horizontal movement
        player_body.velocity = player_body.get_real_x_velocity()
            + player_body.get_real_y_velocity() * 0.05f;
    }

    float dot_vel = player_body.get_real_dot_y_velocity();
    // not sure if we want to ignore this when dashing
    if (dot_vel >= JUMP_SPEED_CAP && !player_state.dashing) {
        player_body.velocity =
            FumoVec2Normalize(player_body.get_real_y_velocity())
                * JUMP_SPEED_CAP
            + player_body.get_real_x_velocity();
    }

    // UpdateCameraCenterSmoothFollow(fumo_engine->camera.get(),
    //                                player_body.position);

    raycast_jumping(player_body, player_capsule, player_state);

    player_body.position += player_body.velocity * fumo_engine->frametime;
    player_capsule.update_capsule_positions(player_body);

    // debug_player_drawing(player_capsule, player_body);
    //-----------------------------------------------------------------
    // scale down the velocity to prepare for
    // the calculations on the next frame
    if (player_state.dashing) {
        player_body.velocity = player_body.get_dash_x_velocity() * 0.75f
            + player_body.get_dash_y_velocity();
        return;
    }
    player_body.velocity = player_body.get_real_x_velocity() * 0.75f
        + player_body.get_real_y_velocity();
}
