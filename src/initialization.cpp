#include "initialization.hpp"

#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_raylib.hpp"

void Initialization::initialize_window() {
    // TODO: scale the window appropriately based on the monitor
    // we are on, among other things
    // (set up the correct game scale)
    InitWindow(screenWidth, screenHeight, "THIS... is a BUCKET.");
}

void FumoEngine::initialize(EngineMode mode, EngineState state) {
    engine_state = state;
    engine_mode = mode;
    ECS = std::make_shared<SchedulerECS>();
    ECS->initialize();
    sprite_manager = std::make_unique<SpriteManager>();
    event_handler = std::make_unique<EntityEventHandler>();
}
