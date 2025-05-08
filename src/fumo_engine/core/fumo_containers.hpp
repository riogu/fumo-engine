#pragma once
#include "fumo_engine/core/fumo_engine.hpp"
// #include <libassert/assert.hpp>
#include <string_view>
#include <unordered_map>

extern std::unique_ptr<GlobalState> global;

template<typename T>
class NamedComponentContainer {
    // friend struct Sprite2DAnimations;
    // NOTE: Associative container used for naming entity_ids which own a component
    // Example: naming SpriteSheet2D like "player_jump" or "player_run"
  private:
    std::unordered_map<std::string_view, EntityId> named_entity_ids {};
    std::string_view component_type_name = libassert::type_name<T>();

  public:
    // enforce type checking on the added entity id

    template<typename U>
    void add_component_by_name(std::string_view entity_name, U component) {

        EntityId entity_id = fumo_engine->ECS->create_entity();
        fumo_engine->ECS->entity_add_components(entity_id, component);

        add_entity_id<U>(entity_id, entity_name);
    }

    // WARNING: this function wont destroy the associated entity_id!!!
    void remove_component_by_name(std::string_view entity_name) {
        size_t erased_count = named_entity_ids.erase(entity_name);
        DEBUG_ASSERT(erased_count != 0,
                     "this id wasn't in this container.",
                     component_type_name);
    }

    // void destroy_component_by_name(std::string_view entity_name) {
    //     EntityId entity_id = named_entity_ids[entity_name];
    //
    //     size_t erased_count = named_entity_ids.erase(entity_name);
    //     DEBUG_ASSERT(erased_count != 0, "this id wasnt in this container.",
    //                  component_type_name);
    //
    //     fumo_engine->ECS->destroy_entity(entity_id);
    // }

    [[nodiscard]] T& get_component_by_name(std::string_view entity_name) {

        DEBUG_ASSERT(named_entity_ids.contains(entity_name),
                     "this component name hasn't been added to this container.",
                     entity_name,
                     named_entity_ids);

        return fumo_engine->ECS->get_component<T>(named_entity_ids[entity_name]);
    }

  private:
    template<typename U>
    void add_entity_id(EntityId entity_id, std::string_view entity_name) {
        auto val = std::is_same_v<T, U>;
        DEBUG_ASSERT(
            val,
            "cant add an entity_id associated with a component of a different type.",
            component_type_name);

        // make sure this component is actually in this entity_id
        // fumo_engine->ECS->check_for_component<U>(entity_id);

        named_entity_ids.insert({entity_name, entity_id});
    }
};

// template<typename T>
// class EntityIdContainer {
//     // NOTE: no direct usecase yet for this container (its useful for storing generic
//     // repetitive components)
//     // Example: storing multiple bombs in one player_id
//     //
//     // a container to store multiple instances of one component,
//     // by associating it with an EntityId, and storing that ID here
//   public:
//     std::vector<EntityId> entity_id_vec{};
//     std::string_view component_type_name = libassert::type_name<T>();
//
//     template<typename U>
//     void add_entity_id(EntityId entity_id) {
//         auto val = std::is_same_v<T, U>;
//         DEBUG_ASSERT(
//             val,
//             "cant add an entity_id associated with a component of a different type.",
//             component_type_name);
//         entity_id_vec.push_back(entity_id);
//     }
//     // WARNING: this function wont destroy the associated entity_id!!!
//     template<typename U>
//     void remove_entity_id(EntityId entity_id) {
//         auto val = std::is_same_v<T, U>;
//         DEBUG_ASSERT(
//             val,
//             "cant remove an entity_id associated with a component of a different
//             type.", component_type_name);
//         entity_id_vec.erase(
//             std::find(entity_id_vec.begin(), entity_id_vec.end(), entity_id));
//     }
// };

// template <class T, class S, class C>
// S& Container(std::priority_queue<T, S, C>& q) {
//     struct HackedQueue : private std::priority_queue<T, S, C> {
//         static S& Container(std::priority_queue<T, S, C>& q) {
//             return q.*&HackedQueue::c;
//         }
//     };
//     return HackedQueue::Container(q);
// }
// template<class T>
// class PQV : public std::priority_queue<T> {
//   public:
//     typedef std::vector<T> TVec;
//     TVec getVector() {
//         TVec r(this->c.begin(), this->c.end());
//         // c is already a heap
//         sort_heap(r.begin(), r.end(), this->comp);
//         // Put it into priority-queue order:
//         reverse(r.begin(), r.end());
//         return r;
//     }
// };
