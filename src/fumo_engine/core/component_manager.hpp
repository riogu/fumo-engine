#pragma once
#include <memory>

#include "cereal/archives/json.hpp"
#include "component_array.hpp"
#include "constants/constants.hpp"
#include "constants/engine_constants.hpp"
#include "system_base.hpp"

// each type needs to map to a respective id
// NOTE: (components are never unregistered in this implementation)
class ComponentManager {
  private:
    std::unordered_map<std::string_view, ComponentId> component_ids;
    std::unordered_map<std::string_view, std::shared_ptr<IComponentArray>>
        component_arrays;
    ComponentId current_component_id {};

    // used for printing whole entities,
    // and for getting all the components of an entity
    std::unordered_map<ComponentId, std::string_view> component_id_to_name;

  public:
    template<typename T>
    void register_component() {
        std::string_view t_name = libassert::type_name<T>();
        DEBUG_ASSERT(!component_ids.contains(t_name),
                     "already registered this component",
                     component_ids);
        // assign the name a unique id (used for the component bitmasks)
        component_ids.insert({t_name, current_component_id});
        // associate the id to the name for debugging
        component_id_to_name.insert({current_component_id, t_name});

        // use the t_name also for creating a new array of this component
        component_arrays.insert({t_name, std::make_shared<ComponentArray<T>>()});

        current_component_id++;
    }

    template<typename T>
    [[nodiscard]] ComponentId get_component_id() {
        std::string_view t_name = libassert::type_name<T>();
        DEBUG_ASSERT(component_ids.contains(t_name),
                     "forgot to register component, or you asked for a system.",
                     component_ids,
                     current_component_id);
        // we use this component id to make component bitmasks
        return component_ids[t_name];
    }

    template<typename T>
    T& get_component(EntityId entity_id) {
        std::string_view t_name = libassert::type_name<T>();
        DEBUG_ASSERT(component_ids.contains(t_name),
                     "forgot to register component",
                     component_ids,
                     t_name);
        std::shared_ptr<ComponentArray<T>> cast_component_array =
            std::static_pointer_cast<ComponentArray<T>>(component_arrays[t_name]);
        return cast_component_array->get_component_data(entity_id);
    }

    template<typename T>
    void replace_component(const EntityId& entity_id, T& new_component) {
        std::string_view t_name = libassert::type_name<T>();
        DEBUG_ASSERT(component_ids.contains(t_name),
                     "forgot to register component",
                     component_ids,
                     t_name);
        std::shared_ptr<ComponentArray<T>> cast_component_array =
            std::static_pointer_cast<ComponentArray<T>>(component_arrays[t_name]);
        cast_component_array->replace_component_data(entity_id, new_component);
    }

    void serialize_component(const EntityId& entity_id,
                             const ComponentId& component_id,
                             cereal::JSONOutputArchive& out_archive) {
        const auto& component_name = component_id_to_name[component_id];
        component_arrays[component_name]->serialize_component(entity_id,
                                                              component_id,
                                                              component_name,
                                                              out_archive);
    }

    template<typename T>
    void get_component_error_internal(EntityId entity_id) {
        std::string_view t_name = libassert::type_name<T>();
        DEBUG_ASSERT(component_ids.contains(t_name),
                     "forgot to register component",
                     component_ids,
                     t_name);
    }

    // responible for changing the component mask of the entity
    template<typename T>
    void add_component(EntityId entity_id, T component) {
        std::string_view t_name = libassert::type_name<T>();
        auto var = std::is_base_of_v<System, T>;
        DEBUG_ASSERT(var != true,
                     "cant register a system as a component.",
                     t_name,
                     component);
        DEBUG_ASSERT(component_ids.contains(t_name),
                     "forgot to register component",
                     component_ids);

        std::static_pointer_cast<ComponentArray<T>>(component_arrays[t_name])
            ->add_component_data(entity_id, component);
    }

    // responsible for changing the component mask of the entity
    template<typename T>
    [[nodiscard]] ComponentId remove_component(EntityId entity_id) {
        std::string_view t_name = libassert::type_name<T>();
        DEBUG_ASSERT(component_ids.contains(t_name),
                     "forgot to register component",
                     component_ids);
        std::shared_ptr<ComponentArray<T>> cast_component_array =
            std::static_pointer_cast<ComponentArray<T>>(component_arrays[t_name]);
        cast_component_array->remove_component_data(entity_id);
        return component_ids[t_name];
    }

    void notify_destroyed_entity(EntityId entity_id) {
        // erases destroyed entity from all sets
        for (auto const& pair : component_arrays) {
            auto const& component_array = pair.second;
            // notice we can erase even if the element wasnt there
            // (simplifying the implementation so i dont go insane)
            component_array->entity_destroyed(entity_id);
        }
    }

    void debug_print() { PRINT(component_arrays); }

    [[nodiscard]] std::string_view
    get_name_of_component_id(ComponentId component_id) {
        return component_id_to_name[component_id];
    }
};
