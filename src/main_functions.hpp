#pragma once
#include "constants/engine_constants.hpp"
#include "fumo_engine/components.hpp"

void debug_print_animation_info(const AnimationInfo& animation_info);
void UpdateCameraCenterSmoothFollow(Camera2D* camera, const FumoVec2& position);
void debug_player_drawing(const Capsule& player_shape, const Body& player_body);

void PRINT_COMPONENT_NAME(const ComponentId& component_id);
