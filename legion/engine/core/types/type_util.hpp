#pragma once
#include <typeinfo>
#include <string>
#include <string_view>

#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>
#include <core/platform/platform.hpp>
#include <core/common/string_extra.hpp>

#include <Optick/optick.h>

/**
 * @file type_util.hpp
 */

namespace legion::core
{
    /**@brief Forcefully casts non pointer/reference type value from one to another.
    */
    template<typename T, typename U>
    constexpr T force_value_cast(U value)
    {
        static_assert(alignof(T) == alignof(U), "Illegal cast of unaligned types.");
        static_assert(sizeof(T) <= sizeof(U), "Illegal cast of non size similar types.");

        return *reinterpret_cast<std::remove_reference_t<T>*>(&value);
    }

    template<typename T, typename U>
    constexpr T* force_cast(const U& value)
    {
        return reinterpret_cast<T*>(&value);
    }

    template<typename T, typename U>
    constexpr T* force_cast(U&& value)
    {
        return reinterpret_cast<T*>(&value);
    }

    template<typename T, typename U>
    constexpr T* force_cast(U* value)
    {
        return reinterpret_cast<T*>(value);
    }


    // Credits to Tristeon engine by Leon and Tristan: https://github.com/Tristeon/Tristeon2D/blob/master/src/Serialization/Type.h
#if defined(LEGION_MSVC) || defined(LEGION_CLANG_MSVC)
    namespace detail
    {
        template<typename T>
        std::string getNameOfTypeImpl()
        {
            OPTICK_EVENT();
            const std::string name = typeid(T).name();
            std::vector<std::string> vec = common::split_string_at<' '>(name);
            if (vec.size() < 2)
            {
                return name;
            }

            std::string result;
            for (auto i = 1; i < vec.size(); i++)
            {
                if (common::ends_with(vec[i], "struct"))
                    result += vec[i].substr(0, vec[i].size() - 6);
                else if (common::ends_with(vec[i], "class"))
                    result += vec[i].substr(0, vec[i].size() - 5);
                else
                    result += vec[i];
            }
            return result;
        }
    }
#elif defined(LEGION_GCC)
}
#include <cxxabi.h>
namespace legion::core
{
    namespace detail
    {
        template<typename T>
        std::string getNameOfTypeImpl()
        {
            OPTICK_EVENT();
            //Demangle the typename generated by GCC
            int status;
            char* realName = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
            std::string result = { realName };
            free(realName);
            return result;
        }
    }
#elif defined(LEGION_CLANG_GCC)
}
#include <cxxabi.h>
namespace legion::core
{
    namespace detail
    {
        template<typename T>
        std::string getNameOfTypeImpl()
        {
            OPTICK_EVENT();
            //Demangle the typename generated by GCC
            int status;
            char* realName = __cxxabiv1::__cxa_demangle(typeid(T).name(), 0, 0, &status);
            std::string result = { realName };
            free(realName);
            return result;
        }
    }
#endif

    /**@brief Returns compile-time evaluable type name.
     * @warning This version is not compiler agnostic! If you need it to be compiler agnostic use `nameOfType`.
     */
    template<typename T>
    constexpr std::string_view localNameOfType() noexcept
    {
#if defined(LEGION_CLANG) || defined(LEGION_GCC)
        cstring p = __PRETTY_FUNCTION__;

        while (*p++ != '=');

        for (; *p == ' '; ++p);

        cstring p2 = p;
        int count = 1;

        for (;; ++p2)
        {
            switch (*p2)
            {
            case '[':
                ++count;
                break;
            case ']':
            case ';':
                --count;
                if (!count)
                    return { p, size_type(p2 - p) };
            }
        }

        return {};
#elif defined(LEGION_MSVC)
        cstring p = __FUNCSIG__;

        while (*p != 'T' || *(p + 1) != 'y' || *(p + 2) != 'p' || *(p + 3) != 'e' || *(p + 4) != '<')
            p++;

        while (*p++ != ' ');

        cstring p2 = p;
        int count = 1;
        size_type size = 0;

        for (; size == 0; ++p2)
        {
            switch (*p2)
            {
            case '<':
                ++count;
                break;
            case '>':
                --count;
                if (!count)
                {
                    size = (p2 - p);
                }
            }
        }

        return { p, size };
#else
#error unknown compiler
#endif
    }

    /**@brief Returns type name with namespaces other than that it's undecorated.
     * @tparam T type of which you want the name.
     */
    template<typename T>
    cstring nameOfType()
    {
        OPTICK_EVENT();
        static std::string name = detail::getNameOfTypeImpl<T>();
        return name.c_str();
    }

    /**@brief Returns type name with namespaces other than that it's undecorated.
     * @tparam T type of which you want the name.
     * @param expr Variable of which you wish to auto deduct type.
     */
    template<typename T>
    cstring nameOfType(T&& expr)
    {
        return nameOfType<std::decay_t<T>>();
    }

    /**@brief Returns hash of a certain string
     * @tparam N Length of the string literal
     * @param name Name you wish to hash
     * @note Since this version takes a const char[] it can only really be used with data coming from a string literal.
     *       Because it takes in a const char[] this function is able to be constexpr and thus have minimal overhead.
     */
    template<size_type N>
    constexpr id_type nameHash(const char(&name)[N]) noexcept
    {
        id_type hash = 0xcbf29ce484222325;
        constexpr uint64 prime = 0x00000100000001b3;

        size_type size = N;
        if (name[size - 1] == '\0')
            size--;

        for (size_type i = 0; i < size; i++)
        {
            hash = hash ^ static_cast<const byte>(name[i]);
            hash *= prime;
        }

        return hash;
    }

    /**@brief Returns hash of a certain string
     * @tparam N Length of the string literal
     * @param name Name you wish to hash
     * @note Since this version takes a const char[] it can only really be used with data coming from a string literal.
     *       Because it takes in a const char[] this function is able to be constexpr and thus have minimal overhead.
     */
    template<size_type N>
    constexpr id_type nameHash(const std::array<char, N>& name) noexcept
    {
        id_type hash = 0xcbf29ce484222325;
        constexpr uint64 prime = 0x00000100000001b3;

        size_type size = N;
        if (name[size - 1] == '\0')
            size--;

        for (int i = 0; i < size; i++)
        {
            hash = hash ^ static_cast<const byte>(name[i]);
            hash *= prime;
        }

        return hash;
    }

    /**@brief Returns hash of a certain string
     * @param name Name you wish to hash
     */
    constexpr id_type nameHash(cstring name) noexcept
    {
        id_type hash = 0xcbf29ce484222325;
        constexpr uint64 prime = 0x00000100000001b3;

        for (size_type i = 0; i < common::constexpr_strlen(name); i++)
        {
            hash = hash ^ static_cast<const byte>(name[i]);
            hash *= prime;
        }

        return hash;
    }

    /**@brief Returns hash of a certain string
     * @param name Name you wish to hash
     */
    id_type nameHash(const std::string& name);

    /**@brief Returns hash of a certain string
     * @param name Name you wish to hash
     */
    constexpr id_type nameHash(const std::string_view& name) noexcept
    {
        id_type hash = 0xcbf29ce484222325;
        constexpr uint64 prime = 0x00000100000001b3;

        size_type size = name.size();

        if (name[size - 1] == '\0')
            size--;

        for (size_type i = 0; i < size; i++)
        {
            hash = hash ^ static_cast<const byte>(name[i]);
            hash *= prime;
        }

        return hash;
    }

    namespace detail
    {
        template<typename T>
        id_type typeHashImpl()
        {
            return nameHash(nameOfType<T>());
        }
    }

    /**@brief Returns compile-time evaluable hash of the type name.
     * @warning This version is not compiler agnostic! If you need it to be compiler agnostic use `typeHash`.
     */
    template<typename T>
    constexpr id_type localTypeHash() noexcept
    {
        return nameHash(localNameOfType<T>());
    }

    /**@brief Returns compiler agnostic hash of the type name.
     * @tparam T type of which you want the hash.
     */
    template<typename T>
    id_type typeHash()
    {
        static id_type hash = detail::typeHashImpl<T>();
        return hash;
    }

    /**@brief Returns hash of the type name.
     * @tparam T type of which you want the hash.
     * @param expr Variable of which you wish to auto deduct type.
     */
    template<typename T>
    id_type typeHash(T expr)
    {
        return typeHash<remove_cvr_t<T>>();
    }

    template<typename Iterator>
    void appendBinaryData(Iterator first, Iterator last, byte_vec& data);

    template<typename T>
    void appendBinaryData(T* value, byte_vec& data)
    {
        OPTICK_EVENT();
        if constexpr (has_resize<std::remove_const_t<T>, void(const std::size_t)>::value)
        {
            auto first = value->begin();
            auto last = value->end();

            uint64 arrSize = std::distance(first, last) * sizeof(typename decltype(first)::value_type);

            for (int i = 0; i < sizeof(uint64); i++)
                data.push_back(reinterpret_cast<const byte*>(&arrSize)[i]);

            for (auto it = first; it != last; ++it)
                appendBinaryData(&*it, data);
        }
        else
        {
            for (int i = 0; i < sizeof(T); i++)
                data.push_back(reinterpret_cast<const byte*>(value)[i]);
        }
    }

    template<typename Iterator>
    void appendBinaryData(Iterator first, Iterator last, byte_vec& data)
    {
        OPTICK_EVENT();
        uint64 arrSize = std::distance(first, last) * sizeof(typename Iterator::value_type);
        appendBinaryData(&arrSize, data);

        for (Iterator it = first; it != last; ++it)
            appendBinaryData(&*it, data); // dereference iterator to get reference, then get the address to get a pointer.
    }

    template<typename T>
    void retrieveBinaryData(T& value, byte_vec::const_iterator& start);

    template<typename Iterator>
    void retrieveBinaryData(Iterator first, Iterator last, byte_vec::const_iterator& start);

    template<typename T>
    uint64 retrieveArraySize(byte_vec::const_iterator start)
    {
        OPTICK_EVENT();
        uint64 arrSize;
        retrieveBinaryData(arrSize, start);
        if (arrSize % sizeof(T) == 0)
            return arrSize / sizeof(T);
        return 0;
    }

    template<typename T>
    void retrieveBinaryData(T& value, byte_vec::const_iterator& start)
    {
        OPTICK_EVENT();
        if constexpr (has_resize<T, void(std::size_t)>::value)
        {
            uint64 arrSize = retrieveArraySize<typename T::value_type>(start);
            value.resize(arrSize);

            retrieveBinaryData(value.begin(), value.end(), start);
        }
        else
        {
            memcpy(&value, &*start, sizeof(T));

            start += sizeof(T);
        }
    }

    template<typename Iterator>
    void retrieveBinaryData(Iterator first, Iterator last, byte_vec::const_iterator& start)
    {
        OPTICK_EVENT();
        uint64 arrSize;
        retrieveBinaryData(arrSize, start);

        uint64 dist = std::distance(first, last) * sizeof(typename Iterator::value_type);
        if (dist > arrSize)
            dist = arrSize;

        Iterator valueIt = first;

        for (byte_vec::const_iterator it = start; it != (start + dist); ++valueIt)
        {
            retrieveBinaryData(*valueIt, it);
        }

        start += arrSize;
    }

}
