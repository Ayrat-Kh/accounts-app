#pragma once

#include <string>
#include <vector>
#include <type_traits>

#include <bsoncxx/builder/basic/document.hpp>
#include <boost/mp11.hpp>
#include <boost/callable_traits/return_type.hpp>

#include "utils/date.hpp"
#include "shared/types.hpp"

namespace app::utils
{
    using namespace bsoncxx::v_noabi;

    template <typename T>
    struct is_vector : std::false_type
    {
    };

    template <typename... Args>
    struct is_vector<std::vector<Args...>> : std::true_type
    {
    };

    template <class T>
    T fromMongoDocument(document::view view);

    template <class TElement>
    struct FromMongoHelpers
    {
        template <class T>
        static T callScalar(const TElement &element);

        template <>
        std::optional<std::string> callScalar(const TElement &element)
        {
            return element.type() == bsoncxx::type::k_string ? std::move(std::string(element.get_string().value)) : std::optional<std::string>{};
        }
        template <>
        std::string callScalar(const TElement &element)
        {
            auto res = std::move(callScalar<std::optional<std::string>>(element));
            return res.has_value() ? std::move(res.value()) : 0;
        }

        template <>
        std::optional<double> callScalar(const TElement &element)
        {
            return element.type() == bsoncxx::type::k_double ? element.get_int64().value : std::optional<double>{};
        }
        template <>
        double callScalar(const TElement &element)
        {
            auto res = std::move(callScalar<std::optional<double>>(element));
            return res.has_value() ? res.value() : 0.f;
        }

        template <>
        std::optional<std::int64_t> callScalar(const TElement &element)
        {
            return element.type() == bsoncxx::type::k_int64 ? element.get_int32().value : std::optional<std::int64_t>{};
        }
        template <>
        std::int64_t callScalar(const TElement &element)
        {
            auto res = std::move(callScalar<std::optional<std::int64_t>>(element));
            return res.has_value() ? res.value() : 0.f;
        }

        template <>
        std::optional<std::int32_t> callScalar(const TElement &element)
        {
            return element.type() == bsoncxx::type::k_int32 ? element.get_int32().value : std::optional<std::int32_t>{};
        }
        template <>
        std::int32_t callScalar(const TElement &element)
        {
            auto res = std::move(callScalar<std::optional<std::int32_t>>(element));
            return res.has_value() ? res.value() : 0.f;
        }

        template <>
        std::optional<app::shared::Datetime> callScalar(const TElement &element)
        {
            return element.type() == bsoncxx::type::k_date
                       ? std::chrono::system_clock::from_time_t(0) + std::chrono::milliseconds(element.get_date().to_int64())
                       : std::optional<app::shared::Datetime>{};
        }
        template <>
        app::shared::Datetime callScalar(const TElement &element)
        {
            auto res = std::move(callScalar<std::optional<app::shared::Datetime>>(element));
            return res.has_value() ? res.value() : app::shared::Datetime{};
        }

        template <class T>
        static T callVector(const TElement &element)
        {
            T result = {};

            if (element.type() != bsoncxx::type::k_array)
            {
                return std::move(result);
            }

            for (auto &elem : element.get_array().value)
            {
                result.push_back(std::move(FromMongoHelpers<bsoncxx::array::element>::handleSerializer<typename T::value_type>(elem)));
            }

            return std::move(result);
        }

        template <class T>
        static T callObject(const TElement &element)
        {
            if (element.type() == bsoncxx::type::k_document)
            {
                return std::move(fromMongoDocument<T>(element.get_document().value));
            }
            return std::move(T{});
        }

        template <class TReturn>
        static constexpr TReturn handleSerializer(const TElement &element)
        {
            if constexpr (
                std::is_same<
                    TReturn, std::string>::value ||
                std::is_same<
                    TReturn, std::optional<std::string>>::value ||
                std::is_same<TReturn, std::int32_t>::value ||
                std::is_same<TReturn, std::optional<std::int32_t>>::value ||
                std::is_same<TReturn, std::int64_t>::value ||
                std::is_same<TReturn, std::optional<std::int64_t>>::value ||
                std::is_same<TReturn, double>::value ||
                std::is_same<TReturn, std::optional<double>>::value ||
                std::is_same<TReturn, shared::Datetime>::value ||
                std::is_same<TReturn, std::optional<shared::Datetime>>::value)
            {
                return std::move(callScalar<TReturn>(element));
            }
            else if constexpr (is_vector<TReturn>::value)
            {
                return std::move(callVector<TReturn>(element));
            }
            else if constexpr (std::is_class<TReturn>::value)
            {
                return std::move(callObject<TReturn>(element));
            }

            assert((void("Uncovered type"), 0));
        }
    };

    template <class T>
    T fromMongoDocument(document::view view)
    {
        using D1 = boost::describe::describe_members<T, boost::describe::mod_public>;
        T result;

        boost::mp11::mp_for_each<D1>(
            [&view, &result](auto D)
            {
                using return_t = std::remove_reference<boost::callable_traits::return_type_t<decltype(D.pointer)>>::type;

                document::element element = view[D.name];

                result.*D.pointer = std::move(FromMongoHelpers<document::element>::handleSerializer<return_t>(element));
                // static_assert(std::is_same<std::string, return_t>{}, "");
            });

        return std::move(result);
    }
}