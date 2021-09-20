#include <core/serialization/serializer.hpp>
#pragma once

namespace legion::core::serialization
{
    namespace detail
    {
        inline bool serialize_ent_data(const ecs::entity_data& ent_data, serializer_view& s_view, std::string& name)
        {
            s_view.start_object(name);
            s_view.serialize("name", ent_data.name);

            s_view.serialize("alive", ent_data.alive);
            s_view.serialize("active", ent_data.active);

            pointer<serializer<ecs::entity_set>> _serializer = serializer_registry::get_serializer<ecs::entity_set>();
            std::string container_name = "children";
            _serializer->serialize_container(&ent_data.children, s_view, container_name);

            s_view.start_container("components");
            auto ent_composition = ecs::Registry::entityComposition(ent_data.id);

            for (id_type typeId : ent_composition)
            {
                auto ent = ecs::Registry::getEntity(ent_data.id);;
                auto _serializer = serializer_registry::get_serializer(typeId);
                auto comp = ecs::Registry::getComponent(typeId, ent);
                std::string compName = ecs::Registry::getFamilyName(typeId);
                std::string empty;
                s_view.start_object(empty);
                _serializer->serialize(comp, s_view, compName);
                s_view.end_object();
            }
            s_view.end_container();

            s_view.end_object();
            return true;
        }
    }

    inline bool serializer<ecs::entity_data>::serialize(const void* ent, serializer_view& s_view, std::string name)
    {
        return detail::serialize_ent_data(*static_cast<const ecs::entity_data*>(ent), s_view, name);
    }

    inline bool serializer<ecs::entity>::serialize(const void* ent, serializer_view& s_view, std::string name)
    {
        return detail::serialize_ent_data(
            *static_cast<const ecs::entity_data*>(static_cast<const ecs::entity*>(ent)->data)
            , s_view, name);
    }

    template<typename type>
    inline bool serializer<type>::serialize_container(const void* container, serializer_view& s_view,std::string& name)
    {
        using container_type = typename remove_cvr_t<type>;
        using value_type = remove_cvr_t<typename std::iterator_traits<container_type::const_iterator>::value_type>;

        auto _container = static_cast<container_type*>(const_cast<void*>(container));

        s_view.start_container(name);

        for (container_type::const_iterator it = _container->begin(); it != _container->end(); ++it)
        {

            pointer<serializer<value_type>> _serializer = serializer_registry::get_serializer<value_type>();

            _serializer->serialize(&*it, s_view, name);
        }

        s_view.end_container();

        return true;
    }

    template<typename type>
    inline bool serializer<type>::serialize(const void* serializable, serializer_view& s_view, std::string name)
    {
        using serializable_type = typename remove_cvr_t<type>;

        if constexpr (is_serializable<serializable_type>::value)
        {
            auto _serializable = *static_cast<const serializable_type*>(serializable);
            s_view.serialize<serializable_type>(name, _serializable);
        }
        else if constexpr (is_container<type>::value)
            serialize_container(serializable, s_view, name);
        else
        {
            auto _serializable = *static_cast<const type*>(serializable);
            auto reflector = make_reflector(_serializable);

            s_view.start_object(name);

            for_each(reflector,
                [&s_view](auto& name, auto& value)
                {
                    using value_type = typename remove_cvr_t<decltype(value)>;

                    auto _serializer = serializer_registry::get_serializer<value_type>();
                    _serializer->serialize(&value, s_view, name);
                });

            s_view.end_object();
        }
        return true;
    }

    template<typename type>
    inline bool serializer<type>::write(const void* serializable, std::string name, fs::view& file)
    {
        auto jsonView = serialization::json_view();
        jsonView.data.append("{");
        auto result = serialize(serializable, jsonView, name);
        jsonView.data[jsonView.data.size() - 1] = '}';

       return file.set(basic_resource(data));

        json j = json::parse(jsonView.data);

        std::ofstream of(fs::view_util::get_view_path(file, true));
        of << j.dump(4);
        of.close();

        return true;
    }

    inline prototype_base serializer<ecs::entity_data>::deserialize(serializer_view& view)
    {
        return prototype_base();
    }

    inline prototype_base serializer<ecs::entity>::deserialize(serializer_view& view)
    {
        return prototype_base();
    }

    template<typename serializable_type>
    inline prototype_base serializer<serializable_type>::deserialize(serializer_view& view)
    {
        return prototype_base();
    }

    inline bool read(const fs::view& view)
    {
        return false;
    }
}
