#include "fumo_engine/core/fumo_engine.hpp"
extern std::unique_ptr<FumoEngine> fumo_engine;
#include "fumo_engine/level_editor/screen_handler.hpp"

void ScreenTransitionUpdater::update_all_screen_ids() {
    const auto& transition_handler =
        fumo_engine->ECS->get_system<ScreenTransitionHandler>();
    for (const auto& transition_line_ids : transition_handler->sys_entities) {
        update_screen_id(transition_line_ids);
    }
}

void ScreenTransitionUpdater::update_screen_id(
    const EntityId& transition_line_id) {
    // we assign a direction (bottom to up, left to right)
    // and that decides which one is the previous screen and the next_screen
    // and then we also assign screens based on the 2 closest ids
    // FIXME: not working, test out this class and fix it
    //
    // mask: only Screen components to check all of them
    // NOTE: has no sys_call() method (gets called by transition handler)

    auto& transition_data =
        fumo_engine->ECS->get_component<ScreenTransitionData>(
            transition_line_id);
    const auto& transition_body =
        fumo_engine->ECS->get_component<Body>(transition_line_id);

    struct ScreenDistance {
        Screen screen;
        float distance = 696969.69;
    };

    // FIXME: fix the fact that the 2 closest screens arent picked correctly

    std::pair<ScreenDistance, ScreenDistance> closest_screens {
        ScreenDistance {.screen = transition_data.previous_screen},
        ScreenDistance {.screen = transition_data.next_screen}};

    for (const auto& entity_id : sys_entities) {
        const auto& screen = fumo_engine->ECS->get_component<Screen>(entity_id);
        // PRINT(screen.screen_id)
        // PRINT(screen.screen_position)
        ScreenDistance new_screen_dist {
            .screen = screen,
            .distance = FumoVec2Distance(screen.screen_position,
                                         transition_body.position)};

        if (new_screen_dist.distance < closest_screens.first.distance)
            closest_screens.first = new_screen_dist;
        else if (new_screen_dist.distance < closest_screens.second.distance)
            closest_screens.second = new_screen_dist;
    }
    if (closest_screens.first.screen.screen_id
            == transition_data.previous_screen.screen_id
        && closest_screens.second.screen.screen_id
            == transition_data.next_screen.screen_id) {
        // PRINT("nothing happened");
        return;
    }
    // PRINT("SOMETHING HAPPENED")

    FumoVec2 direction_vec =
        direction_to_vector(transition_data.transition_direction);

    FumoVec2 direction1 =
        FumoVec2Normalize(closest_screens.first.screen.screen_position
                          - transition_data.transition_line.start);
    FumoVec2 direction2 =
        FumoVec2Normalize(closest_screens.second.screen.screen_position
                          - transition_data.transition_line.start);
    // DEBUG
    BeginMode2D(fumo_engine->fumo_camera->camera);
    Line {.start = closest_screens.first.screen.screen_position,
          .end =
              transition_data.transition_line.start + transition_body.position}
        .draw(FUMO_BLUE, {0.0f, 0.0f});
    Line {.start = closest_screens.second.screen.screen_position,
          .end =
              transition_data.transition_line.start + transition_body.position}
        .draw(FUMO_BLUE, {0.0f, 0.0f});
    EndMode2D();

    FumoVec2DotProduct(direction1, direction_vec)
            > FumoVec2DotProduct(direction2, direction_vec)
        ? (transition_data.previous_screen = closest_screens.second.screen,
           transition_data.next_screen = closest_screens.first.screen)
        : (transition_data.previous_screen = closest_screens.first.screen,
           transition_data.next_screen = closest_screens.second.screen);

    PRINT(transition_data.previous_screen.screen_id)
    PRINT(transition_data.next_screen.screen_id)
}
