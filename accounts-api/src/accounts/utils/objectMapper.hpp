#pragma once

#include <type_traits>

#include <boost/describe.hpp>

#include "accounts/shared/types.hpp"

namespace accounts
{
    template <
        class TTo, class TFrom,
        class TToMembers = boost::describe::describe_members<
            TTo,
            boost::describe::mod_public | boost::describe::mod_inherited>,
        class TFromMembers = boost::describe::describe_members<
            TFrom,
            boost::describe::mod_public | boost::describe::mod_inherited>>
    TTo remapObject(TFrom object);

    template <
        class TTo, class TFrom>
    static constexpr TTo handleTypeMap(TFrom object, const char *fieldName)
    {
        using real_from_t = typename unwrap_optional<TFrom>::type;
        using real_to_t = typename unwrap_optional<TTo>::type;

        if constexpr (std::is_same<TTo, TFrom>::value)
        {
            return std::move(object);
        }

        if constexpr (
            is_primitive<real_to_t>::value &&
            std::is_same<real_from_t, real_to_t>::value)
        {
            return std::move(object);
        }
        else if constexpr (is_vector<real_to_t>::value && is_vector<real_from_t>::value)
        {
            using vector_item_to_t = typename real_to_t::value_type;
            using vector_item_from_t = typename real_from_t::value_type;

            TTo container;
            container.reserve(object.size());

            for (auto &&item : object)
            {
                if constexpr (
                    is_primitive<vector_item_to_t>::value &&
                    std::is_same<vector_item_to_t, vector_item_from_t>::value)
                {
                    container.push_back(std::move(item));
                    continue;
                }
                else if constexpr (
                    std::is_class<vector_item_to_t>::value &&
                    std::is_class<vector_item_from_t>::value &&
                    !is_primitive<vector_item_to_t>::value &&
                    !is_primitive<vector_item_from_t>::value &&
                    !is_vector<vector_item_to_t>::value &&
                    !is_vector<vector_item_from_t>::value)
                {
                    container.push_back(std::move(remapObject<vector_item_to_t, vector_item_from_t>(std::move(item))));
                    continue;
                }
            }
            return std::move(container);
        }
        else if constexpr (
            std::is_class<real_to_t>::value &&
            std::is_class<real_from_t>::value &&
            !is_primitive<real_to_t>::value &&
            !is_primitive<real_from_t>::value &&
            !is_vector<real_to_t>::value &&
            !is_vector<real_from_t>::value)
        {

            return std::move(remapObject<TTo, TFrom>(std::move(object)));
        }

        return TTo{};
    }

    template <
        class TTo, class TFrom,
        class TToMembers,
        class TFromMembers>
    TTo remapObject(TFrom object)
    {
        TTo result;

        boost::mp11::mp_for_each<TFromMembers>(
            [&result, &object](auto DFrom)
            {
                boost::mp11::mp_for_each<TToMembers>(
                    [&result, &object, &DFrom](auto DTo)
                    {
                        if (DFrom.name != DTo.name)
                        {
                            return;
                        }

                        result.*DTo.pointer = std::move(
                            handleTypeMap<
                                typename pointer_return_type<decltype(DTo.pointer)>::type,
                                typename pointer_return_type<decltype(DFrom.pointer)>::type>(
                                std::move(object.*DFrom.pointer),
                                DTo.name));
                    });
            });

        return std::move(result);
    }

}