#include "fumo_engine/collisions_and_physics/collision_functions.hpp"
#include "fumo_engine/core/fumo_engine.hpp"
extern std::unique_ptr<FumoEngine> fumo_engine;
#include "fumo_engine/level_editor/screen_handler.hpp"
#include "fumo_engine/screen_components.hpp"
#include "main_functions.hpp"

[[nodiscard]] bool fumovec2_almost_equals(FumoVec2 p, FumoVec2 q) {
    // Check whether two given vectors are almost equal

#if defined(EPSILON)
    #undef EPSILON
    #define EPSILON 0.01f
#endif

    int result = ((fabsf(p.x - q.x))
                  <= (EPSILON * fmaxf(1.0f, fmaxf(fabsf(p.x), fabsf(q.x)))))
        && ((fabsf(p.y - q.y))
            <= (EPSILON * fmaxf(1.0f, fmaxf(fabsf(p.y), fabsf(q.y)))));

#undef EPSILON
#define EPSILON 0.000001f
    return result;
}

void ScreenTransitionHandler::check_for_screen_transition() {

    const auto& player_id = fumo_engine->player_id;
    const auto& player_capsule =
        fumo_engine->ECS->get_component<Capsule>(fumo_engine->player_id);
    const auto& player_body =
        fumo_engine->ECS->get_component<Body>(fumo_engine->player_id);
    auto& player_state =
        fumo_engine->ECS->get_component<EntityState>(fumo_engine->player_id);

    bool collided = false;

    // PRINT(fumo_engine->fumo_camera->camera.target.x)
    // PRINT(fumo_engine->fumo_camera->camera.target.y)

    const auto& screen_updater =
        fumo_engine->ECS->get_system<ScreenTransitionUpdater>();

    // go through all ScreenTransitionRect
    for (const auto& entity_id : sys_entities) {
        auto& transition_line =
            fumo_engine->ECS->get_component<ScreenTransitionData>(entity_id);
        // assign the correct screens to the screen transition line based
        // on the which ones are closer (and assigned direction)

        // screen_updater->update_screen_id(entity_id);

        if (Collisions::LineToCapsuleCollided(
                player_capsule,
                player_body,
                transition_line.transition_line,
                fumo_engine->ECS->get_component<Body>(entity_id))) {

            collided = true;

            PRINT_NO_NAME("WE COLLIDED WITH LINE", "YEAH")

            fumo_engine->engine_state = EngineState::GAMEPLAY_PAUSED;

            if (fumo_engine->fumo_camera->last_transition_id != entity_id) {

                // if (fumo_engine->fumo_camera->current_screen.screen_id
                //     != transition_line.next_screen.screen_id) {
                fumo_engine->fumo_camera->current_screen =
                    transition_line.next_screen;
                std::swap(transition_line.previous_screen,
                          transition_line.next_screen);
                // }
            }

            fumo_engine->fumo_camera->last_transition_id = entity_id;
        }
    }

    if (!collided) fumo_engine->fumo_camera->last_transition_id = -69;

    if (fumovec2_almost_equals(
            to_fumo_vec2(fumo_engine->fumo_camera->camera.target),
            fumo_engine->fumo_camera->current_screen.screen_position)) {
        fumo_engine->engine_state = EngineState::GAMEPLAY_RUNNING;
    }

    if (to_fumo_vec2(fumo_engine->fumo_camera->camera.target)
        != fumo_engine->fumo_camera->current_screen.screen_position) {
        UpdateCameraCenterSmoothFollow(
            &fumo_engine->fumo_camera->camera,
            fumo_engine->fumo_camera->current_screen.screen_position);
    }
}

namespace FumoEvent {

void screen_transition(const Event& event) {
    // const auto& new_screen =
    //     fumo_engine->ECS->get_component<Screen>(event.entity_id);
    // do stuff to the camera here and all that
    // ...how tf do you do that
    // force the player to free in place until the camera is set
    // with a boolean or a disabling event
    // entity_state.is_changing_screens = true;
}
} // namespace FumoEvent
