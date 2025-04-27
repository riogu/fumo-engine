#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_engine/serialization/fumo_serializer.hpp"
#include "initialization.hpp"
void debug_spawn_level_objects();

#define XMACRO(Type) EntityQuery Type##_query {};
ALL_COMPONENTS_X_MACRO()
#undef XMACRO

int count = 0;

std::unique_ptr<FumoEngine> fumo_engine;

int main(void) {
    Initialization::initialize_window();
    //--------------------------------------------------------------------------------------
    fumo_engine = std::make_unique<FumoEngine>();
    fumo_engine->initialize(EngineMode::LEVEL_EDITING, EngineState::EDITING);
    //--------------------------------------------------------------------------------------
    // must be done before fumo_engine->setup_game();
    Initialization::initialize_all_textures();
    // all components MUST be added to ALL_COMPONENTS_X_MACRO()
    // to be registered into fumo_engine
    Initialization::register_all_to_fumo_engine();
    //--------------------------------------------------------------------------------------
    fumo_engine->setup_game();
    FumoSerializer::deserialize_levels();
    //--------------------------------------------------------------------------------------

    // if (!count) [[unlikely]] {
    //     count++;
    //     debug_spawn_level_objects();
    // }
    while (fumo_engine->engine_state != EngineState::SHOULD_CLOSE) {
        fumo_engine->frame_loop();
    }
    //--------------------------------------------------------------------------------------
    // unload textures before closing the OpenGL context
    // Close window and OpenGL context
    fumo_engine->destroy_and_unload_game();
    //--------------------------------------------------------------------------------------

    return 0;
}

// here because we start with no planets right now (remove when we make levels)
