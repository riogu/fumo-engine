#include "constants/planet_constants.hpp"
#include "fumo_engine/core/fumo_engine.hpp"
#include "fumo_engine/event_components.hpp"
#include "fumo_engine/level_editor/level_editor.hpp"

extern std::unique_ptr<FumoEngine> fumo_engine;
#define XMACRO(Type) extern EntityQuery Type##_query;
ALL_COMPONENTS_X_MACRO()
#undef XMACRO

void DebugLevelEditor::find_selection(FumoVec2 mouse_position) {
#define XMACRO(Type) \
    if (fumo_engine->ECS->filter(entity_id, Type##_query)) { \
        DebugLevelEditor::find_selection_object( \
            entity_id, \
            fumo_engine->ECS->get_component<Type>(entity_id), \
            mouse_position, \
            fumo_engine->ECS->get_component<Body>(entity_id).position); \
        continue; \
    }

    EntityId previous_selected = currently_selected_entity;

    for (const auto& entity_id : sys_entities) {

        if (entity_id == selection_rectangle_id) continue;

        if (fumo_engine->ECS->filter(entity_id, EditorSelectedObject_query)
            || (IsKeyDown(KEY_LEFT_SHIFT))) {
            FIND_SELECTION_ALL_COMPONENTS_X_MACRO()
        }
    }

    if (currently_selected_entity == selection_rectangle_id) return;
    if (previous_selected != currently_selected_entity) {
        // changed selected object
        if (fumo_engine->ECS->filter(previous_selected,
                                     EditorSelectedObject_query)) {
            fumo_engine->ECS->entity_remove_component<EditorSelectedObject>(
                previous_selected);
        }
        if (!fumo_engine->ECS->filter(currently_selected_entity,
                                      EditorSelectedObject_query)) {
            fumo_engine->ECS->entity_add_components(currently_selected_entity,
                                                    EditorSelectedObject {});
        }
    }

#undef XMACRO
}

void DebugLevelEditor::move_screen_to_mouse(FumoVec2 mouse_position) {
    fumo_engine->fumo_camera->camera.target =
        FumoVec2SmoothMoveTowards({fumo_engine->fumo_camera->camera.target.x,
                                   fumo_engine->fumo_camera->camera.target.y},
                                  mouse_position,
                                  18.5f)
            .to_raylib_vec2();
}

template<>
void DebugLevelEditor::find_selection_object(const EntityId& entity_id,
                                             FumoRect& shape,
                                             FumoVec2& mouse_position,
                                             FumoVec2& body_position) {

    FumoRect collision_bounds {.x = body_position.x,
                               .y = body_position.y,
                               .width = shape.width,
                               .height = shape.height};

    if (CheckCollisionPointRec(mouse_position.to_raylib_vec2(),
                               collision_bounds.to_raylib_rect())
        || entity_id == currently_selected_entity) {

        auto& selection_rect = fumo_engine->ECS->get_component<OutlineRect>(
            selection_rectangle_id);
        selection_rect.outline_rect = shape;
        auto& selection_body =
            fumo_engine->ECS->get_component<Body>(selection_rectangle_id);
        selection_body.position = {collision_bounds.x, collision_bounds.y};

        currently_selected_entity = entity_id;

        if (IsKeyDown(KEY_LEFT_SHIFT)) return;

        if (IsKeyDown(KEY_SPACE)) {
            mouse_position = {mouse_position.x - shape.width / 2.0f,
                              mouse_position.y - shape.height / 2.0f};
            body_position =
                FumoVec2SmoothMoveTowards(body_position, mouse_position, 4.0f);
        }

        if (IsKeyDown(KEY_LEFT_CONTROL)) {

            float new_width = (mouse_position.x - body_position.x);
            float new_height = (mouse_position.y - body_position.y);
            shape.width += (new_width - shape.width) / 3.0f;
            shape.height += (new_height - shape.height) / 3.0f;

            if (shape.width < MINIMUM_OBJECT_SIZE) {
                shape.width = MINIMUM_OBJECT_SIZE;
            }
            if (shape.height < MINIMUM_OBJECT_SIZE) {
                shape.height = MINIMUM_OBJECT_SIZE;
            }
        }
    }
}

template<>
void DebugLevelEditor::find_selection_object(const EntityId& entity_id,
                                             float& radius,
                                             FumoVec2& mouse_position,
                                             FumoVec2& body_position) {

    FumoRect collision_bounds {.x = body_position.x - radius,
                               .y = body_position.y - radius,
                               .width = radius * 2.0f,
                               .height = radius * 2.0f};

    if (CheckCollisionPointRec(mouse_position.to_raylib_vec2(),
                               collision_bounds.to_raylib_rect())
        || entity_id == currently_selected_entity) {

        if (IsKeyDown(KEY_SPACE)) {
            body_position =
                FumoVec2SmoothMoveTowards(body_position, mouse_position, 4.0f);
        }

        if (!(IsKeyDown(KEY_LEFT_SHIFT)) && IsKeyDown(KEY_LEFT_CONTROL)) {
            float new_radius = (mouse_position.x - body_position.x);
            radius += (new_radius - radius) / 3.0f;

            if (radius < MINIMUM_OBJECT_SIZE) {
                radius = MINIMUM_OBJECT_SIZE;
            }
        }

        auto& selection_rect = fumo_engine->ECS->get_component<OutlineRect>(
            selection_rectangle_id);
        selection_rect.outline_rect = collision_bounds;
        auto& selection_body =
            fumo_engine->ECS->get_component<Body>(selection_rectangle_id);
        selection_body.position = {collision_bounds.x, collision_bounds.y};

        currently_selected_entity = entity_id;
    }
}

template<>
void DebugLevelEditor::find_selection_object(const EntityId& entity_id,
                                             ScreenTransitionData& shape,
                                             FumoVec2& mouse_position,
                                             FumoVec2& body_position) {

    FumoRect collision_bounds {
        .x = body_position.x,
        .y = body_position.y,
        .width = FumoVec2Distance(shape.transition_line.start,
                                  shape.transition_line.end),
        .height = FumoVec2Distance(shape.transition_line.start,
                                   shape.transition_line.end)};
    FumoVec2 prev_position = body_position;

    if (CheckCollisionPointLine(
            mouse_position.to_raylib_vec2(),
            (shape.transition_line.start + body_position).to_raylib_vec2(),
            (shape.transition_line.end + body_position).to_raylib_vec2(),
            1000.0f)
        || entity_id == currently_selected_entity) {

        if (IsKeyDown(KEY_SPACE)) {
            body_position =
                FumoVec2SmoothMoveTowards(body_position, mouse_position, 4.0f);
        }

        if (!(IsKeyDown(KEY_LEFT_SHIFT)) && IsKeyDown(KEY_LEFT_CONTROL)) {
            FumoVec2 new_line_end =
                (mouse_position - (shape.transition_line.end + body_position));

            shape.transition_line.end += (new_line_end) / 3.0f;

            if (FumoVec2Distance(shape.transition_line.start,
                                 shape.transition_line.end)
                < MINIMUM_OBJECT_SIZE) {
                shape.transition_line.end = shape.transition_line.start
                    + FumoVec2Normalize(shape.transition_line.end)
                        * MINIMUM_OBJECT_SIZE;
            }
        }

        auto& selection_rect = fumo_engine->ECS->get_component<OutlineRect>(
            selection_rectangle_id);
        selection_rect.outline_rect = collision_bounds;
        auto& selection_body =
            fumo_engine->ECS->get_component<Body>(selection_rectangle_id);
        selection_body.position = {collision_bounds.x, collision_bounds.y};

        currently_selected_entity = entity_id;
    }
}

template<>
void DebugLevelEditor::find_selection_object(const EntityId& entity_id,
                                             ParallelGravityField& shape,
                                             FumoVec2& mouse_position,
                                             FumoVec2& body_position) {
    DebugLevelEditor::find_selection_object(entity_id,
                                            shape.field_fumo_rect,
                                            mouse_position,
                                            body_position);
}

template<>
void DebugLevelEditor::find_selection_object(const EntityId& entity_id,
                                             OutlineRect& shape,
                                             FumoVec2& mouse_position,
                                             FumoVec2& body_position) {
    DebugLevelEditor::find_selection_object(entity_id,
                                            shape.outline_rect,
                                            mouse_position,
                                            body_position);
}

template<>
void DebugLevelEditor::find_selection_object(const EntityId& entity_id,
                                             Circle& shape,
                                             FumoVec2& mouse_position,
                                             FumoVec2& body_position) {
    DebugLevelEditor::find_selection_object(entity_id,
                                            shape.radius,
                                            mouse_position,
                                            body_position);
}

template<>
void DebugLevelEditor::find_selection_object(const EntityId& entity_id,
                                             CircularGravityField& shape,
                                             FumoVec2& mouse_position,
                                             FumoVec2& body_position) {
    DebugLevelEditor::find_selection_object(entity_id,
                                            shape.radius,
                                            mouse_position,
                                            body_position);
}
