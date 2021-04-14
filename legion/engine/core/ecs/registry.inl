#include <core/ecs/registry.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type, typename... Args>
    inline L_ALWAYS_INLINE component_pool<component_type>* Registry::tryEmplaceFamily(Args&&... args)
    {
        OPTICK_EVENT();
        if (getFamilies().count(make_hash<component_type>())) // Check and fetch in order to avoid a possibly unnecessary allocation and deletion.
            return static_cast<component_pool<component_type>*>(getFamilies().at(make_hash<component_type>()).get());

        // Allocate and emplace if no item was found.
        return static_cast<component_pool<component_type>*>(
            getFamilies().emplace(
                make_hash<component_type>(),
                std::unique_ptr<component_pool_base>(new component_pool<component_type>(std::forward<Args>(args)...))
            ).first->second.get() // std::pair<iterator, bool>.first --> iterator<std::pair<key, value>>->second --> std::unique_ptr.get() --> component_pool_base* 
            );
    }

    template<typename component_type, typename... Args>
    inline L_ALWAYS_INLINE void ecs::Registry::registerComponentType(Args&&... args)
    {
        OPTICK_EVENT();
        getFamilies().try_emplace(
            make_hash<component_type>(),
            std::unique_ptr<component_pool_base>(new component_pool<component_type>(std::forward<Args>(args)...))
        );
    }

    template<typename component_type0, typename component_type1, typename... component_types, typename... Args>
    inline L_ALWAYS_INLINE void ecs::Registry::registerComponentType(Args&&... args)
    {
        registerComponentType<component_type0>(std::forward<Args>(args)...);
        registerComponentType<component_type1, component_types...>(std::forward<Args>(args)...);
    }

    template<typename component_type, typename... Args>
    inline L_ALWAYS_INLINE component_pool<component_type>* ecs::Registry::getFamily(Args&&... args)
    {
        OPTICK_EVENT();
        return tryEmplaceFamily<component_type>(std::forward<Args>(args)...);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& Registry::createComponent(entity target)
    {
        OPTICK_EVENT();
        // Check and emplace component family if it doesn't exist yet.
        static bool checked = false; // Prevent unnecessary unordered_map lookups.
        if (!checked && !getFamilies().count(make_hash<component_type>()))
        {
            checked = true;
            registerComponentType<component_type>();
        }

        // Update entity composition.
        entityCompositions().at(target).insert(make_hash<component_type>());
        // Update filters.
        FilterRegistry::markComponentAdd<component_type>(target);
        // Actually create and return the component. (this uses the direct function which avoids use of virtual indirection)
        return component_pool<component_type>::create_component_direct(target);
    }


    template<typename component_type0, typename component_type1, typename... component_typeN>
    inline L_ALWAYS_INLINE std::tuple<component_type0&, component_type1&, component_typeN&...> Registry::createComponent(entity target)
    {
        return std::make_tuple(std::ref(createComponent<component_type0>(target)), std::ref(createComponent<component_type1>(target)), std::ref(createComponent<component_typeN>(target))...);
    }

    template<typename archetype_type>
    inline L_ALWAYS_INLINE typename archetype_type::refGroup Registry::createComponent(entity target)
    {
        return archetype_type::create(target);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& Registry::createComponent(entity target, component_type&& value)
    {
        OPTICK_EVENT();
        // Check and emplace component family if it doesn't exist yet.
        static bool checked = false; // Prevent unnecessary unordered_map lookups.
        if (!checked && !getFamilies().count(make_hash<component_type>()))
        {
            checked = true;
            registerComponentType<component_type>();
        }

        // Update entity composition.
        entityCompositions().at(target).insert(make_hash<component_type>());
        // Update filters.
        FilterRegistry::markComponentAdd<component_type>(target);
        // Actually create and return the component. (this uses the direct function which avoids use of virtual indirection)
        return component_pool<component_type>::create_component_direct(target, std::forward<component_type>(value));
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& Registry::createComponent(entity target, const component_type& value)
    {
        OPTICK_EVENT();
        // Check and emplace component family if it doesn't exist yet.
        static bool checked = false; // Prevent unnecessary unordered_map lookups.
        if (!checked && !getFamilies().count(make_hash<component_type>()))
        {
            checked = true;
            registerComponentType<component_type>();
        }

        // Update entity composition.
        entityCompositions().at(target).insert(make_hash<component_type>());
        // Update filters.
        FilterRegistry::markComponentAdd<component_type>(target);
        // Actually create and return the component. (this uses the direct function which avoids use of virtual indirection)
        return component_pool<component_type>::create_component_direct(target, value);
    }

    template<typename archetype_type>
    inline typename archetype_type::refGroup Registry::createComponent(entity target, archetype_type&& value)
    {
        return archetype_type::create(target, std::forward<archetype_type>(value));
    }

    template<typename archetype_type>
    inline typename archetype_type::refGroup Registry::createComponent(entity target, const archetype_type& value)
    {
        return archetype_type::create(target, value);
    }

    template<typename archetype_type, typename component_type0, typename component_type1, typename... component_typeN>
    inline typename archetype_type::refGroup Registry::createComponent(entity target, component_type0&& value0, component_type1&& value1, component_typeN&&... valueN)
    {
        return archetype_type::create(target, std::forward<component_type0>(value0), std::forward<component_type1>(value1), std::forward<component_typeN>(valueN)...);
    }

    template<typename archetype_type, typename component_type0, typename component_type1, typename... component_typeN>
    inline typename archetype_type::refGroup Registry::createComponent(entity target, const component_type0& value0, const component_type1& value1, const component_typeN&... valueN)
    {
        return archetype_type::create(target, value0, value1, valueN...);
    }

    template<typename component_type0, typename component_type1, typename... component_typeN>
    inline std::tuple<component_type0&, component_type1&, component_typeN&...> Registry::createComponent(entity target, component_type0&& value0, component_type1&& value1, component_typeN&&... valueN)
    {
        return std::make_tuple(std::ref(createComponent<component_type0>(target, std::forward<component_type0>(value0))),
                               std::ref(createComponent<component_type1>(target, std::forward<component_type1>(value1))),
                               std::ref(createComponent<component_typeN>(target, std::forward<component_typeN>(valueN)))...);
    }

    template<typename component_type0, typename component_type1, typename... component_typeN>
    inline std::tuple<component_type0&, component_type1&, component_typeN&...> Registry::createComponent(entity target, const component_type0& value0, const component_type1& value1, const component_typeN&... valueN)
    {
        return std::make_tuple(std::ref(createComponent<component_type0>(target, value0)),
                               std::ref(createComponent<component_type1>(target, value1)),
                               std::ref(createComponent<component_typeN>(target, valueN))...);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& Registry::createComponent(entity target, const serialization::component_prototype<component_type>& prototype)
    {
        OPTICK_EVENT();
        // Check and emplace component family if it doesn't exist yet.
        static bool checked = false; // Prevent unnecessary unordered_map lookups.
        if (!checked && !getFamilies().count(make_hash<component_type>()))
        {
            checked = true;
            registerComponentType<component_type>();
        }

        // Update entity composition.
        entityCompositions().at(target).insert(make_hash<component_type>());
        // Update filters.
        FilterRegistry::markComponentAdd<component_type>(target);
        // Actually create and return the component using the prototype. (this uses the direct function which avoids use of virtual indirection)
        return component_pool<component_type>::create_component_direct(target, prototype);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& Registry::createComponent(entity target, serialization::component_prototype<component_type>&& prototype)
    {
        OPTICK_EVENT();
        // Check and emplace component family if it doesn't exist yet.
        static bool checked = false; // Prevent unnecessary unordered_map lookups.
        if (!checked && !getFamilies().count(make_hash<component_type>()))
        {
            checked = true;
            registerComponentType<component_type>();
        }

        // Update entity composition.
        entityCompositions().at(target).insert(make_hash<component_type>());
        // Update filters.
        FilterRegistry::markComponentAdd<component_type>(target);
        // Actually create and return the component using the prototype. (this uses the direct function which avoids use of virtual indirection)
        return component_pool<component_type>::create_component_direct(target, std::move(prototype));
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void Registry::destroyComponent(entity target)
    {
        OPTICK_EVENT();
        // Check and emplace component family if it doesn't exist yet.
        static bool checked = false; // Prevent unnecessary unordered_map lookups.
        if (!checked && !getFamilies().count(make_hash<component_type>()))
        {
            checked = true;
            registerComponentType<component_type>();
        }

        // Update entity composition.
        entityCompositions().at(target).erase(make_hash<component_type>());
        // Update filters.
        FilterRegistry::markComponentErase<component_type>(entity{ &Registry::entityData(target) });
        // Actually destroy the component. (this uses the direct function which avoids use of virtual indirection)
        component_pool<component_type>::destroy_component_direct(target);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE bool Registry::hasComponent(entity target)
    {
        OPTICK_EVENT();
        // Check and emplace component family if it doesn't exist yet.
        static bool checked = false; // Prevent unnecessary unordered_map lookups.
        if (!checked && !getFamilies().count(make_hash<component_type>()))
        {
            checked = true;
            registerComponentType<component_type>();
        }
        // Check if a component is existent. (this uses the direct function which avoids use of virtual indirection)
        return component_pool<component_type>::contains_direct(target);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& Registry::getComponent(entity target)
    {
        OPTICK_EVENT();
        // Check and emplace component family if it doesn't exist yet.
        static bool checked = false; // Prevent unnecessary unordered_map lookups.
        if (!checked && !getFamilies().count(make_hash<component_type>()))
        {
            checked = true;
            registerComponentType<component_type>();
        }

        // Fetch the component. (this uses the direct function which avoids use of virtual indirection)
        return component_pool<component_type>::get_component_direct(target);
    }

}
