#pragma once
// very useful for generating code related to our components
// XMACRO(Type) can be redefined to generate code with each Type
#define ALL_COMPONENTS_X_MACRO() \
    XMACRO(Body) \
    XMACRO(Circle) \
    XMACRO(AnimationInfo) \
    XMACRO(Timer) \
    XMACRO(Render) \
    XMACRO(FumoRect) \
    XMACRO(Capsule) \
    XMACRO(ParallelGravityField) \
    XMACRO(CircularGravityField) \
    XMACRO(ColliderObjectFlag) \
    XMACRO(GravFieldFlag) \
    XMACRO(OutlineRect) \
    XMACRO(EntityState) \
    XMACRO(MovedEventData) \
    XMACRO(Serialized) \
    XMACRO(LevelId) \
    XMACRO(ScreenTransitionData) \
    XMACRO(Line) \
    XMACRO(CollisionEventData) \
    XMACRO(EditorSelectedObject) \
    XMACRO(Screen)

#define XMACRO(Type) Type,
enum struct AllComponentTypes { ALL_COMPONENTS_X_MACRO() };
#undef XMACRO // NOTE: dont forget to undefine the xmacro
