#include "component_renderer.hpp"
#include "fumo_engine/core/entity_query.hpp"
#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_engine/level_editor/level_editor.hpp"
#include "fumo_engine/screen_components.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;
// required to access the queries
#define XMACRO(Type) extern EntityQuery Type##_query;
ALL_COMPONENTS_X_MACRO()
#undef XMACRO

#define DRAW_ALL_COMPONENTS_X_MACRO() \
    XMACRO(Circle) \
    XMACRO(FumoRect) \
    XMACRO(Capsule) \
    XMACRO(ParallelGravityField) \
    XMACRO(CircularGravityField) \
    XMACRO(OutlineRect) \
    XMACRO(ScreenTransitionData) \
    XMACRO(Line)

void ComponentRenderer::draw_all_components() {
    DrawFPS(10, 10);
    BeginMode2D(fumo_engine->fumo_camera->camera);

#define XMACRO(Type) \
    if (fumo_engine->ECS->filter(entity_id, Type##_query)) { \
        fumo_engine->ECS->get_component<Type>(entity_id).draw( \
            fumo_engine->ECS->get_component<Render>(entity_id).color, \
            fumo_engine->ECS->get_component<Body>(entity_id).position); \
        continue; \
    }

    for (const auto& entity_id : sys_entities) {
        DRAW_ALL_COMPONENTS_X_MACRO()
    }

    // draw the selection on top of all the others
    // const auto& selection_rect_id =
    //     fumo_engine->ECS->get_system<DebugLevelEditor>()
    //         ->selection_rectangle_id;
    // fumo_engine->ECS->get_component<OutlineRect>(selection_rect_id)
    //     .draw(
    //         fumo_engine->ECS->get_component<Render>(selection_rect_id).color,
    //         fumo_engine->ECS->get_component<Body>(selection_rect_id).position);

#undef XMACRO // dont forget to undefine the xmacro
    EndMode2D();
}
