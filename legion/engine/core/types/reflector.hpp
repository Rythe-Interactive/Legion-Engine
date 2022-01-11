#pragma once
#include <core/platform/platform.hpp>
#include <core/types/meta.hpp>
#include <core/types/primitives.hpp>

namespace legion::core
{
    struct member_reference;
    //a reference to data
    struct reflector
    {
        id_type typeId;
        std::string_view typeName;
        std::vector<member_reference> members;
        void* data;

        reflector() = default;
        reflector(id_type id, std::string_view name, std::vector<member_reference> _members, void* _adress)
            : typeId(id), typeName(name), members(_members), data(_adress) {}
        reflector(const reflector& refl) : typeId(refl.typeId), typeName(refl.typeName), members(refl.members), data(refl.data) {}

        reflector& operator=(const reflector& rhs)
        {
            typeId = rhs.typeId;
            typeName = rhs.typeName;
            members = rhs.members;
            data = rhs.data;

            return *this;
        }
    };
    struct primitive_reference
    {
        id_type typeId;
        void* data = nullptr;

        template<typename T>
        L_NODISCARD T* cast()
        {
            if (localTypeHash<T>() == typeId)
            {
                return reinterpret_cast<T*>(data);
            }
            return nullptr;
        }

        template<typename T>
        L_NODISCARD const T* cast() const
        {
            if (localTypeHash<T>() == typeId)
            {
                return reinterpret_cast<const T*>(data);
            }
            return nullptr;
        }
    };
    struct member_reference
    {
        bool is_object;
        std::string_view name;
        union
        {
            reflector object;
            primitive_reference primitive;
        };

        member_reference() : is_object(false), name(""), primitive() {}
        member_reference(std::string_view _name, primitive_reference _primitive) : is_object(false), name(_name), primitive(_primitive) {}
        member_reference(std::string_view _name, reflector refl) : is_object(true), name(_name), object(refl) {}
        member_reference(const member_reference& other)
        {
            is_object = other.is_object;
            name = other.name;
            if (is_object)
                new (&object) reflector(other.object);
            else
                new (&primitive) primitive_reference(other.primitive);
        }
        ~member_reference()
        {
            if (is_object)
                object.~reflector();
            else
                primitive.~primitive_reference();
        }

        member_reference& operator=(const member_reference& other)
        {
            if (is_object)
                object.~reflector();
            else
                primitive.~primitive_reference();

            is_object = other.is_object;
            name = other.name;
            if (is_object)
                new (&object) reflector(other.object);
            else
                new (&primitive) primitive_reference(other.primitive);

            return *this;
        }
    };

    template<typename T>
    L_NODISCARD auto make_reflector(T& obj) -> std::conditional_t<std::is_const_v<T>, const reflector, reflector>
    {
        ptr_type adress = reinterpret_cast<ptr_type>(std::addressof(obj));
        return reflector{ typeHash<T>(), nameOfType<T>(), std::vector<member_reference>(), reinterpret_cast<void*>(adress) };
    }
}
