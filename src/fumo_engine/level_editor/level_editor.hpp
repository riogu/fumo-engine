#pragma once
#include "fumo_engine/components.hpp"
#include "fumo_engine/core/system_base.hpp"
#include "fumo_engine/screen_components.hpp"
#include "fumo_raylib.hpp"

class DebugLevelEditor: public System {

  public:
    EntityId selection_rectangle_id = 1;
    EntityId currently_selected_entity = 1;

    void sys_call() override { handle_input(); }

    void handle_input();

    void resize_selection(FumoVec2 mouse_position);

    void spawn_circular_planet(FumoVec2 mouse_position);
    void spawn_circular_field(FumoVec2 mouse_position);
    void spawn_circle(FumoVec2 mouse_position);

    void spawn_rect(FumoVec2 mouse_position);
    void spawn_rect_field(FumoVec2 mouse_position);
    void spawn_transition_line(FumoVec2 mouse_position);

    void move_entity(FumoVec2 mouse_position);
    void find_selection(FumoVec2 mouse_position);
    void delete_planet(FumoVec2 mouse_position);
    void move_screen_to_mouse(FumoVec2 mouse_position);

    void reset_position();

    void save_level();
    void debug_print();

    template<typename T>
    void find_selection_object(const EntityId& entity_id,
                               T& shape,
                               FumoVec2& mouse_position,
                               FumoVec2& body_position);
    // void spawn_rect_planet(FumoVec2 mouse_position);
};

struct LevelEditorGUI {
    void save_level();
};

#define FIND_SELECTION_ALL_COMPONENTS_X_MACRO() \
    XMACRO(Circle) \
    XMACRO(FumoRect) \
    XMACRO(ParallelGravityField) \
    XMACRO(CircularGravityField) \
    XMACRO(OutlineRect) \
    XMACRO(ScreenTransitionLine)

#define XMACRO(Type) \
    template<> \
    void DebugLevelEditor::find_selection_object(const EntityId& entity_id, \
                                                 Type& shape, \
                                                 FumoVec2& mouse_position, \
                                                 FumoVec2& body_position);

FIND_SELECTION_ALL_COMPONENTS_X_MACRO()

#undef XMACRO

template<>
void DebugLevelEditor::find_selection_object(const EntityId& entity_id,
                                             float& radius,
                                             FumoVec2& mouse_position,
                                             FumoVec2& body_position);
