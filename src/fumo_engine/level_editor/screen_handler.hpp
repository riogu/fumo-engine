#pragma once
#include "fumo_engine/core/system_base.hpp"

struct ScreenTransitionHandler: System {
    // FIXME: finish this class
    void sys_call() override { check_for_screen_transition(); }

    void check_for_screen_transition();
};

struct ScreenTransitionUpdater: System {
    // NOTE: i should be able to click a menu button and type down a screen number
    // and move whatever entity i selected to that screen by doing this
    // (since it will be relative)
    //
    void sys_call() override { update_all_screen_ids(); }

    void update_all_screen_ids();

    // FIXME: current |
    // continuously sweep the game and update the screen id
    // as i build the levels (so i dont have to manually state it)
    void update_screen_id(const EntityId& transition_line_id);
};
