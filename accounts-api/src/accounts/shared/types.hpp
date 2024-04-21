#pragma once

#include <chrono>

#include <boost/callable_traits/return_type.hpp>
#include <type_traits>

namespace accounts
{
    using Datetime = std::chrono::time_point<std::chrono::system_clock>;

    template <typename T>
    struct is_vector : std::false_type
    {
    };

    template <typename... Args>
    struct is_vector<std::vector<Args...>> : std::true_type
    {
    };

    template <typename T>
    struct is_optional : std::false_type
    {
    };

    template <typename... Args>
    struct is_optional<std::optional<Args...>> : std::true_type
    {
    };

    template <typename TArg>
    struct unwrap_optional : std::false_type
    {
        using type = TArg;
    };

    template <typename TArg>
    struct unwrap_optional<std::optional<TArg>>
    {
        using type = TArg;
    };

    template <class TPointer>
    struct pointer_return_type
    {
        using type = std::remove_reference<boost::callable_traits::return_type_t<TPointer>>::type;
    };

    template <class TItem>
    struct is_primitive
    {
        static constexpr bool value =
            std::is_same<TItem, std::string>::value ||
            std::is_same<TItem, std::int32_t>::value ||
            std::is_same<TItem, std::int64_t>::value ||
            std::is_same<TItem, int>::value ||
            std::is_same<TItem, double>::value ||
            std::is_same<TItem, Datetime>::value;
    };
}