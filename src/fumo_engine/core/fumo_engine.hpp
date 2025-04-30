#pragma once
#include <memory>

#include "constants/engine_constants.hpp"
#include "fumo_engine/components.hpp" // IWYU pragma: export
#include "fumo_engine/core/scheduler_ecs.hpp"
#include "fumo_engine/events/event_state_handlers.hpp"
#include "fumo_engine/screen_components.hpp"
#include "fumo_engine/sprite_animation_manager/sprite_and_animation_systems.hpp"

struct FumoCamera {
    Camera2D camera {};
    Screen current_screen {.screen_id = 43, .screen_position = screenCenter};
    EntityId last_transition_id = -43;
};

class FumoEngine {
  public:
    float frametime;
    std::shared_ptr<SchedulerECS> ECS;

    std::unique_ptr<SpriteManager> sprite_manager;

    std::unique_ptr<FumoCamera> fumo_camera;

    std::unique_ptr<EntityEventHandler> event_handler;

    EntityId player_id;
    // NOTE: storing the player id globally for now for optimisation
    // this isnt necessary, and can be removed completely later.

    EngineState engine_state {};

    EngineMode engine_mode {};

    void initialize(EngineMode mode, EngineState state);

    void handle_events() { event_handler->handle_events(); }

    void setup_game();

    void destroy_and_unload_game() {
        // TODO: unload our textures and also destroy all dynamic memory in global
        CloseWindow();
        PRINT_NO_NAME("FUMO: Fumo Engine successfully terminated.");
    }

    void frame_loop() {
        if (WindowShouldClose()) engine_state = EngineState::SHOULD_CLOSE;

        ClearBackground(BLACK);
        BeginDrawing();
        frametime = GetFrameTime();

        switch (engine_mode) {
            case EngineMode::GAMEPLAY:
                ECS->run_systems(engine_state);
                // events are handled after all systems are ran
                if (engine_state == EngineState::GAMEPLAY_RUNNING
                    || engine_state == EngineState::RUN_ALL_DEBUG) {
                    handle_events();
                }
                break;
            case EngineMode::LEVEL_EDITING:
                ECS->run_systems(engine_state);
                // events are handled after all systems are ran
                handle_events();
                break;
        }
        EndDrawing();
    }

    void gameplay_loop() {}

    void level_editing_loop() {}
};
