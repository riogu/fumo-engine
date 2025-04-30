#pragma once
#include <memory>

#include "constants/constants.hpp"
#include "fumo_engine/components.hpp"
#include "fumo_engine/core/scheduler_ecs.hpp"
#include "fumo_engine/core/system_base.hpp"

struct SchedulerSystemECS: System {
    //
    // NOTE: this is an extension of the main SchedulerECS class
    // used to awake systems when necessary, and put them to sleep

    // WARNING:
    // this is for methods commonly accessed by other systems,
    // **NOT** for normal direct access from global to register and such

    // weak ptr so wr get no memory leak warnings when we close the window
    // (this being shared meant that this reference wouldnt get deleted once
    // global_state variable get deleted)
    std::weak_ptr<SchedulerECS> parent_ECS;

    // TODO: make this a raw pointer and make the SchedulerECS a unique_ptr

    SchedulerSystemECS(std::shared_ptr<SchedulerECS> parent_ECS) :
        parent_ECS(parent_ECS) {}

    void sys_call() override {};

    template<typename T>
    void awake_unregistered_system() {
        std::string_view t_name = libassert::type_name<T>();
        const auto& parent_ptr = parent_ECS.lock();
        const auto& system_ptr = parent_ptr->ecs->get_system(t_name);

        DEBUG_ASSERT(
            !parent_ptr->all_scheduled_unregistered_systems_debug.contains(
                t_name),
            "can't awake an unregistered system that isn't asleep.");

        parent_ptr->unregistered_system_scheduler.insert(system_ptr);
    }

    template<typename T, Priority priority>
    void awake_unregistered_system_priority() {
        std::string_view t_name = libassert::type_name<T>();
        const auto& parent_ptr = parent_ECS.lock();
        auto& system_ptr = parent_ptr->ecs->get_system(t_name);
        system_ptr->priority = priority;

        DEBUG_ASSERT(
            !parent_ptr->all_scheduled_unregistered_systems_debug.contains(
                t_name),
            "can't awake an unregistered system that isn't asleep.",
            t_name);

        parent_ptr->unregistered_system_scheduler.insert(system_ptr);
        parent_ptr->all_scheduled_unregistered_systems_debug.insert(
            {t_name, system_ptr});
    }

    template<typename T>
    void sleep_unregistered_system() {
        std::string_view t_name = libassert::type_name<T>();
        const auto& parent_ptr = parent_ECS.lock();
        const auto& system_ptr = parent_ptr->ecs->get_system(t_name);
        size_t erased_count =
            parent_ptr->unregistered_system_scheduler.erase(system_ptr);
        parent_ptr->all_scheduled_unregistered_systems_debug.erase(t_name);
        DEBUG_ASSERT(erased_count != 0,
                     "this unregistered system wasnt awake/scheduled.",
                     t_name);
    }

    template<typename T>
    void sleep_unregistered_system_for(float seconds_duration) {
        std::string_view t_name = libassert::type_name<T>();
        const auto& parent_ptr = parent_ECS.lock();
        const auto& system_ptr = parent_ptr->ecs->get_system(t_name);
        size_t erased_count =
            parent_ptr->unregistered_system_scheduler.erase(system_ptr);
        parent_ptr->all_scheduled_unregistered_systems_debug.erase(t_name);
        DEBUG_ASSERT(erased_count != 0,
                     "this unregistered system wasnt awake/scheduled.",
                     t_name);

        EntityId timer_id = parent_ptr->create_entity();

        Timer timer;
        timer.make_timer(seconds_duration, t_name);

        parent_ptr->entity_add_components(timer_id, timer);
    }

    void awake_unregistered_system_from_name(std::string_view t_name) {
        const auto& parent_ptr = parent_ECS.lock();
        const auto& system_ptr = parent_ptr->ecs->get_system(t_name);

        DEBUG_ASSERT(
            !parent_ptr->all_scheduled_unregistered_systems_debug.contains(
                t_name),
            "can't awake a system that isn't asleep.");

        parent_ptr->unregistered_system_scheduler.insert(system_ptr);
    }

    //------------------------------------------------------------
    // DEBUG: REMOVE THESE LATER
    template<typename T>
    void awake_system() {
        std::string_view t_name = libassert::type_name<T>();
        const auto& parent_ptr = parent_ECS.lock();
        auto& system_ptr = parent_ptr->ecs->get_system(t_name);

        DEBUG_ASSERT(!parent_ptr->all_scheduled_systems_debug.contains(t_name),
                     "can't awake a system that isn't asleep.",
                     t_name);

        parent_ptr->system_scheduler.insert(system_ptr);
        parent_ptr->all_scheduled_systems_debug.insert({t_name, system_ptr});
    }

    template<typename T>
    void sleep_system() {
        std::string_view t_name = libassert::type_name<T>();
        const auto& parent_ptr = parent_ECS.lock();
        const auto& system_ptr = parent_ptr->ecs->get_system(t_name);
        size_t erased_count = parent_ptr->system_scheduler.erase(system_ptr);
        parent_ptr->all_scheduled_systems_debug.erase(t_name);
        DEBUG_ASSERT(erased_count != 0,
                     "this system wasnt awake/scheduled.",
                     t_name);
    }

    //------------------------------------------------------------
};

struct TimerHandler: System {

    void sys_call() override { update_timers(); }

    void update_timers();
};

// scheduling systems are systems that coordinate the starting and ending of systems
// based on things going on with the code
// for example it might run a series of systems for only 6 seconds, based on a game
// condition such as obtaining an item and playing out a cutscene for example
