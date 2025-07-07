# fumo-engine
This is the game engine i made as a wrapper around raylib to add more functionality that is needed to make a scalable game.
The only goal for this engine is making my game based around gravity platformer physics, it is not intended to be used as a standalone engine, and i don't really recommend using it.

Here, first we use the level editor to make a level, and then afterwards we change the engine to gameplay and play it
[<img src = "https://github.com/user-attachments/assets/cbe4b968-dcc7-494d-b6b6-62662f9f4915" width ="100%">](https://github.com/user-attachments/assets/cbe4b968-dcc7-494d-b6b6-62662f9f4915)




example of main.cpp.
```cpp
int main(void) {
    Initialization::initialize_window();
    //-------------------------------------------------------------------------
    fumo_engine = std::make_unique<FumoEngine>();

    // fumo_engine->initialize(EngineMode::LEVEL_EDITING, EngineState::EDITING);
    fumo_engine->initialize(EngineMode::GAMEPLAY, EngineState::GAMEPLAY_RUNNING);
    //
    //------------------------------------------------------------------------
    // must be done before fumo_engine->setup_game();
    Initialization::initialize_all_textures();

    // all components MUST be added to ALL_COMPONENTS_X_MACRO()
    // to be registered into fumo_engine
    Initialization::register_all_to_fumo_engine();

    //-------------------------------------------------------------------------
    fumo_engine->setup_game();
    FumoSerializer::deserialize_levels();
    //-------------------------------------------------------------------------

    while (fumo_engine->engine_state != EngineState::SHOULD_CLOSE) {
        fumo_engine->frame_loop();
    }
    //-------------------------------------------------------------------------
    // unload textures before closing the OpenGL context
    // Close window and OpenGL context
    fumo_engine->destroy_and_unload_game();
    //------------------------------------------------------------------------

    return 0;
}
```
# Implemented features

- ECS
> the game engine's core. Is used to manage game data globally and provide queries for the game's systems.
> mostly follows the traditional ECS format, has packed arrays for components and systems provide the components they want to query upfront and get updated whenever an entity has a change to their components, or gets destroyd.
-  Animation/Sprites
> Based on godot's AnimationPlayer, it is a wrapper around raylib's textures to allow you to register spritesheet pngs and they will be turned into animations for sprites in the game.
- Collisions/Physics
> an okay physics library i made myself for collision detection + solving. handles capsules, lines, rectangles, circles (and also allows for rotations to the rectangles with small modifications). could be adapted to also support polygons, but it wouldn't be very optimal. most collision detection is done with naive methods, but for the purposes of this engine and the games made with it, it is good enough.
> Collision solving is done mostly with shape casting.
- Level editor
> simple editor built into the game specifically related to gravity platformer, it is just used to make it easier to create the levels and place the obstacles on each screen for developing gravity platformer.
- Serialization
> relies on cereal serialization library for this functionality. allows for saving the current game progress, saving levels in the level editor, and just keeping permanent data (with backups) around to help with making the game.
- Events and Scheduler
> the game engine also provides a system for creating events that are consumed every frame. you can also add lasting events that are responsible for destroying themselves once they are done with their task/condition has been met for ending. they can also be set on a timer and so on.
> the scheduler ended up being not very useful, but it allows to add and remove systems dynamically, by setting a timer or a condition for removal

- gravity platformer
> demo game to showcase my idea for an interesting gravity based platformer. meant to showcase fumo engine's functionality and also for prototyping a possible future game idea.

Final note:
> fumo engine is just meant to lay the ground work and add enough functionality to allow me to make a proper finished 2d platformer in the future, it is not meant to be used by other people.
