#pragma once
#include <memory>

#include "constants/engine_constants.hpp"
#include "fumo_engine/core/component_manager.hpp"
#include "fumo_engine/core/entity_manager.hpp"
#include "fumo_engine/core/entity_query.hpp"
#include "fumo_engine/core/system_manager.hpp"

class ECS {
  private:
    std::unique_ptr<EntityManager> entity_manager;
    std::unique_ptr<ComponentManager> component_manager;
    std::unique_ptr<SystemManager> system_manager;

  public:
    void initialize() {
        entity_manager = std::make_unique<EntityManager>();
        component_manager = std::make_unique<ComponentManager>();
        system_manager = std::make_unique<SystemManager>();
    }

    // ------------------------------------------------------------------------------
    // entity stuff
    [[nodiscard]] EntityId create_entity() {
        return entity_manager->create_entity();
    }

    void destroy_entity(EntityId entity_id) {
        entity_manager->destroy_entity(entity_id);
        component_manager->notify_destroyed_entity(entity_id);
        system_manager->notify_destroyed_entity(entity_id);
    }

    void serialize_component(const EntityId& entity_id,
                             ComponentId component_id,
                             cereal::JSONOutputArchive& out_archive) {
        component_manager->serialize_component(entity_id,
                                               component_id,
                                               out_archive);
    }

    // --------------------------------------------------------------------------------------

    // --------------------------------------------------------------------------------------
    // component stuff
    template<typename T>
    void register_component() {
        component_manager->register_component<T>();
    }

    void entity_component_mask_changed(EntityId entity_id,
                                       ComponentMask component_mask) {
        system_manager->entity_component_mask_changed(entity_id,
                                                      component_mask);
    }

    template<typename T>
    void entity_add_component(EntityId entity_id, T component) {
        // tell the component manager to add this entity_id associated to this
        // component this updates the entity's component mask
        component_manager->add_component<T>(entity_id, component);
        // update the entity_id's mask
        ComponentId component_id = get_component_id<T>();
        // add the component
        entity_manager->add_to_component_mask(entity_id, component_id);

        // NOTE: you can now add multiple components at once
    }

    template<typename T> // remove from entity
    void entity_remove_component(EntityId entity_id) {
        auto component_id = component_manager->remove_component<T>(entity_id);

        entity_manager->remove_from_component_mask(entity_id, component_id);

        ComponentMask component_mask =
            entity_manager->get_component_mask(entity_id);
        system_manager->entity_component_mask_changed(entity_id,
                                                      component_mask);
        // return component_mask;
        // NOTE: change this so it wont notify systems until the end of the frame
    }

    template<typename T>
    [[nodiscard]] T& get_component(EntityId entity_id) {
        return component_manager->get_component<T>(entity_id);
    }

    template<typename T>
    void replace_component(const EntityId& entity_id, T& new_component) {
        component_manager->replace_component<T>(entity_id, new_component);
    }

    // template<typename T>
    // T& get_component_from_name(EntityId entity_id, const std::string_view&
    // t_name) {
    //     // component_manager->get_component_from_name<T>(entity_id, t_name);
    // }

    template<typename T>
    void get_component_error_internal(EntityId entity_id) {
        return component_manager->get_component_error_internal<T>(entity_id);
    }

    template<typename T>
    [[nodiscard]] ComponentId get_component_id() {
        return component_manager->get_component_id<T>();
    }

    [[nodiscard]] ComponentMask get_component_mask(EntityId entity_id) {
        return entity_manager->get_component_mask(entity_id);
    }

    // --------------------------------------------------------------------------------------

    // --------------------------------------------------------------------------------------
    // system stuff
    template<typename T>
    void register_system(const EntityQuery& entity_query,
                         const std::shared_ptr<T>& system_ptr) {
        system_manager->register_system<T>(entity_query, system_ptr);
    }

    template<typename T>
    void add_unregistered_system(const std::shared_ptr<T>& system_ptr) {
        system_manager->add_unregistered_system<T>(system_ptr);
    }

    template<typename T>
    void set_system_entity_query(ComponentMask component_mask) {
        system_manager->set_entity_query<T>(component_mask);
    }

    [[nodiscard]] std::shared_ptr<System>&
    get_system(std::string_view type_name) {
        return system_manager->get_system(type_name);
    }

    // --------------------------------------------------------------------------------------
    void debug_print() {
        component_manager->debug_print();
        entity_manager->debug_print();
        system_manager->debug_print();
    }

    [[nodiscard]] std::string_view
    get_name_of_component_id(ComponentId component_id) {
        return component_manager->get_name_of_component_id(component_id);
    }
};
