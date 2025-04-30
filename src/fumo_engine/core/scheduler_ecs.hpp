#pragma once
#include <memory>

#include "entity_query.hpp"
#include "fumo_engine/components.hpp" // IWYU pragma: export
#include "fumo_engine/core/ECS.hpp"
#include "fumo_engine/core/system_base.hpp"
#include "fumo_engine/event_components.hpp" // IWYU pragma: export

struct SystemCompare {
    inline bool operator()(const std::shared_ptr<System>& sysA,
                           const std::shared_ptr<System>& sysB) const {
        return sysA->priority < sysB->priority;
    }
};

class SchedulerECS {
  private:
    friend struct SchedulerSystemECS;
    //------------------------------------------------------------------

    std::multiset<std::shared_ptr<System>, SystemCompare> system_scheduler {};
    std::multiset<std::shared_ptr<System>, SystemCompare>
        unregistered_system_scheduler {};

    //------------------------------------------------------------------
    std::unique_ptr<ECS> ecs;

    // TODO: check if you really need this extra map later
    // (using it for printing rn)
    // (i think it might be unnecessary overhead that slows down the ECS)
    std::unordered_map<std::string_view, std::shared_ptr<System>>
        all_scheduled_unregistered_systems_debug {};
    std::unordered_map<std::string_view, std::shared_ptr<System>>
        all_scheduled_systems_debug {};
    std::array<EntityId, MAX_ENTITY_IDS> all_entity_ids_debug {};
    // std::vector<AllComponentTypes> all_component_types;

  public:
    void initialize() {
        ecs = std::make_unique<ECS>();
        ecs->initialize();
    }

    //------------------------------------------------------------------
    void run_systems(EngineState engine_state) {
        // we copy every frame, so that changes to the
        // scheduler only apply on the next loop iteration
        // unregistered systems go first right now

        std::multiset<std::shared_ptr<System>, SystemCompare> copy_scheduler(
            unregistered_system_scheduler);
        copy_scheduler.insert(system_scheduler.begin(), system_scheduler.end());
        // run unregistered systems
        // and run registered systems
        switch (engine_state) {
            case EngineState::GAMEPLAY_RUNNING:
                for (const auto& system_ptr : copy_scheduler) {
                    if (system_ptr->system_mode
                            == SystemMode::GAMEPLAY_RUNNING_ONLY
                        || system_ptr->system_mode
                            == SystemMode::GAMEPLAY_AND_PAUSED
                        || system_ptr->system_mode == SystemMode::ALWAYS_RUN)
                        system_ptr->sys_call();
                }
                break;
            case EngineState::GAMEPLAY_PAUSED:
                for (const auto& system_ptr : copy_scheduler) {
                    if (system_ptr->system_mode
                            == SystemMode::GAMEPLAY_AND_PAUSED
                        || system_ptr->system_mode == SystemMode::ALWAYS_RUN)
                        system_ptr->sys_call();
                }
                break;
            case EngineState::EDITING:
                for (const auto& system_ptr : copy_scheduler) {
                    if (system_ptr->system_mode == SystemMode::EDITING_ONLY
                        || system_ptr->system_mode == SystemMode::ALWAYS_RUN)
                        system_ptr->sys_call();
                }
                break;
            case EngineState::RUN_ALL_DEBUG:
                for (const auto& system_ptr : copy_scheduler) {
                    system_ptr->sys_call();
                }
                break;
            case EngineState::SHOULD_CLOSE:
                PRINT_NO_NAME("FUMO: Window should close.")
                PRINT_NO_NAME("FUMO: Closing Fumo Engine...");
                break;
        }
    }

    //------------------------------------------------------------------
    // entity stuff
    [[nodiscard]] EntityId create_entity() {
        // added "scheduled" to allude to the fact this action only happens by
        // the end of the frame, can remove later
        EntityId entity_id = ecs->create_entity();
        // scheduler.entities_to_create.insert(entity_id);
        all_entity_ids_debug[entity_id] = entity_id;
        return entity_id;
    };

    void destroy_entity(EntityId entity_id) {
        // schedule deletion for the end of the frame
        // scheduler.entities_to_destroy.insert(entity_id);
        ecs->destroy_entity(entity_id);
        all_entity_ids_debug[entity_id] = NO_ENTITY_FOUND;
    }

    void serialize_component(const EntityId& entity_id,
                             ComponentId component_id,
                             cereal::JSONOutputArchive& out_archive) {
        ecs->serialize_component(entity_id, component_id, out_archive);
    }

    //------------------------------------------------------------------
    // component stuff
    template<typename T>
    void register_component() {
        ecs->register_component<T>();
    }

    // TODO: consider adding components in bulk when creating entities
    // to minimize the number of update calls we make to systems (this really
    // would help)
    // template<typename T>
    // void entity_add_components(const EntityId& entity_id, T component) {
    //     // scheduler.scheduled_entity_component_masks[entity_id] =
    //     ecs->entity_add_components(entity_id, component);
    // }

    template<typename... Types>

    void entity_add_components(const EntityId& entity_id, Types... components) {
        (ecs->entity_add_component(entity_id, components), ...);

        ecs->entity_component_mask_changed(entity_id,
                                           get_component_mask(entity_id));
    }

    template<typename T>
    void replace_or_add_component(const EntityId& entity_id, T component) {
        if (filter(entity_id,
                   EntityQuery {.component_mask = make_component_mask<T>(),
                                .component_filter = Filter::All})) {
            ecs->replace_component(entity_id, component);
            return;
        }
        entity_add_components(entity_id, component);
    }

    template<typename T> // remove from entity
    void entity_remove_component(const EntityId& entity_id) {
        // scheduler.scheduled_entity_component_masks[entity_id] =
        ecs->entity_remove_component<T>(entity_id);
    }

    template<typename T>
    [[nodiscard]] T& get_component(const EntityId& entity_id) {
        return ecs->get_component<T>(entity_id);
    }

    // template<typename T>
    // void check_for_component(const EntityId& entity_id) {
    //     return ecs->check_for_component<T>(entity_id);
    // }
    template<typename T>
    [[nodiscard]] ComponentId get_component_id() {
        return ecs->get_component_id<T>();
    }

    [[nodiscard]] ComponentMask get_component_mask(const EntityId& entity_id) {
        return ecs->get_component_mask(entity_id);
    }

    // --------------------------------------------------------------------------------------

    // --------------------------------------------------------------------------------------
    // system stuff
    template<typename T,
             Priority priority,
             SystemMode system_mode,
             typename... Types>
    void register_system(EntityQuery entity_query, Types&... args) {
        std::shared_ptr<T> system_ptr = std::make_shared<T>(args...);
        ecs->register_system<T>(entity_query, system_ptr);
        system_ptr->priority = priority;
        system_ptr->system_mode = system_mode;

        system_scheduler.insert(system_ptr);

        // NOTE: remove later if not using debugger
        std::string_view t_name = libassert::type_name<T>();
        all_scheduled_systems_debug.insert({t_name, system_ptr});
    }

    // WARNING: this system wont be called in run_systems() call
    template<typename T, SystemMode system_mode, typename... Types>
    void register_system_unscheduled(EntityQuery entity_query, Types&... args) {
        const std::shared_ptr<T> system_ptr = std::make_shared<T>(args...);
        ecs->register_system<T>(entity_query, system_ptr);
        system_ptr->system_mode = system_mode;
        // return system_ptr;
    }

    //------------------------------------------------------------------
    // WARNING: this system will get no updates to it's sys_entities

    template<typename T,
             Priority priority,
             SystemMode system_mode,
             typename... Types>
    void add_unregistered_system(Types&... args) {
        const std::shared_ptr<T> system_ptr = std::make_shared<T>(args...);
        ecs->add_unregistered_system(system_ptr);
        system_ptr->priority = priority;
        system_ptr->system_mode = system_mode;
        // can be changed to be in the constructor or test this with
        // initializer list
        unregistered_system_scheduler.insert(system_ptr);

        // NOTE: remove later if not using debugger
        std::string_view t_name = libassert::type_name<T>();
        all_scheduled_unregistered_systems_debug.insert({t_name, system_ptr});
    }

    // this system wont be called in run_systems() call
    template<typename T, SystemMode system_mode, typename... Types>
    void add_unregistered_system_unscheduled(Types&... args) {
        const std::shared_ptr<T> system_ptr = std::make_shared<T>(args...);
        system_ptr->system_mode = system_mode;
        ecs->add_unregistered_system(system_ptr);

        // unregistered_system_scheduler.insert(system_ptr);
    }

    // returns the system cast from the System interface
    template<typename T>
    [[nodiscard]] std::shared_ptr<T> get_system() {
        std::string_view type_name = libassert::type_name<T>();

        // added so we only need one method to find all systems even if not
        // registered
        return std::static_pointer_cast<T>(ecs->get_system(type_name));
    }

    template<typename... Types>
    [[nodiscard]] ComponentMask make_component_mask() {
        ComponentMask component_mask = 0;
        (add_id_to_component_mask<Types>(component_mask), ...);
        return component_mask;
    }

  private:
    template<typename T>
    void add_id_to_component_mask(ComponentMask& component_mask) {
        auto var = std::is_base_of_v<System, T>;
        DEBUG_ASSERT(var != true,
                     "cant add a system to another system's component mask.");

        component_mask |= 1 << get_component_id<T>();
    }

    // --------------------------------------------------------------------------------------
  public:
    // filtering methods for entity ids
    [[nodiscard]] bool filter(EntityId entity_id, EntityQuery entity_query) {
        return entity_query.filter(ecs->get_component_mask(entity_id));
    }

    void debug_print_entity(EntityId entity_id);
    void debug_print();
    void debug_print_scheduler();
};
