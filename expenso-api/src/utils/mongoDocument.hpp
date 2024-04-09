#pragma once

#include <string>
#include <vector>
#include <type_traits>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/types.hpp>
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

    template <class T>
    static T deserializeMongoDocument(document::view view);

    template <class TElement = document::view>
    struct MongoSerializeHelpers
    {
        template <class T>
        static T deserializeScalar(const TElement &element);

        template <>
        std::optional<std::string> deserializeScalar(const TElement &element)
        {
            return element.type() == bsoncxx::type::k_string ? std::move(std::string(element.get_string().value)) : std::optional<std::string>{};
        }
        template <>
        std::string deserializeScalar(const TElement &element)
        {
            auto res = std::move(deserializeScalar<std::optional<std::string>>(element));
            return res.has_value() ? std::move(res.value()) : 0;
        }

        template <>
        std::optional<double> deserializeScalar(const TElement &element)
        {
            return element.type() == bsoncxx::type::k_double ? element.get_int64().value : std::optional<double>{};
        }
        template <>
        double deserializeScalar(const TElement &element)
        {
            auto res = std::move(deserializeScalar<std::optional<double>>(element));
            return res.has_value() ? res.value() : 0.f;
        }

        template <>
        std::optional<std::int64_t> deserializeScalar(const TElement &element)
        {
            return element.type() == bsoncxx::type::k_int64 ? element.get_int32().value : std::optional<std::int64_t>{};
        }
        template <>
        std::int64_t deserializeScalar(const TElement &element)
        {
            auto res = std::move(deserializeScalar<std::optional<std::int64_t>>(element));
            return res.has_value() ? res.value() : 0.f;
        }

        template <>
        std::optional<std::int32_t> deserializeScalar(const TElement &element)
        {
            return element.type() == bsoncxx::type::k_int32 ? element.get_int32().value : std::optional<std::int32_t>{};
        }
        template <>
        std::int32_t deserializeScalar(const TElement &element)
        {
            auto res = std::move(deserializeScalar<std::optional<std::int32_t>>(element));
            return res.has_value() ? res.value() : 0.f;
        }

        template <>
        std::optional<app::shared::Datetime> deserializeScalar(const TElement &element)
        {
            return element.type() == bsoncxx::type::k_date
                       ? std::chrono::system_clock::from_time_t(0) + std::chrono::milliseconds(element.get_date().to_int64())
                       : std::optional<app::shared::Datetime>{};
        }
        template <>
        app::shared::Datetime deserializeScalar(const TElement &element)
        {
            auto res = std::move(deserializeScalar<std::optional<app::shared::Datetime>>(element));
            return res.has_value() ? res.value() : app::shared::Datetime{};
        }

        template <class T>
        static T deserializeVector(const TElement &element)
        {
            T result = {};

            if (element.type() != bsoncxx::type::k_array)
            {
                return std::move(result);
            }

            for (auto &elem : element.get_array().value)
            {
                result.push_back(std::move(MongoSerializeHelpers<bsoncxx::array::element>::handleTypeDeserialization<typename T::value_type>(elem)));
            }

            return std::move(result);
        }

        template <class T>
        static T deserializeObject(const TElement &element)
        {
            if (element.type() == bsoncxx::type::k_document)
            {
                return std::move(deserializeMongoDocument<T>(element.get_document().value));
            }
            return std::move(T{});
        }

        template <class TReturn>
        static constexpr TReturn handleTypeDeserialization(const TElement &element)
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
                return std::move(deserializeScalar<TReturn>(element));
            }
            else if constexpr (is_vector<TReturn>::value)
            {
                return std::move(deserializeVector<TReturn>(element));
            }
            else if constexpr (std::is_class<TReturn>::value)
            {
                return std::move(deserializeObject<TReturn>(element));
            }

            assert((void("Uncovered type"), 0));
        }
    };

    template <class T>
    static T deserializeMongoDocument(document::view view)
    {
        using D1 = boost::describe::describe_members<T, boost::describe::mod_public>;
        T result;

        boost::mp11::mp_for_each<D1>(
            [&view, &result](auto D)
            {
                using return_t = std::remove_reference<boost::callable_traits::return_type_t<decltype(D.pointer)>>::type;

                document::element element = view[D.name == "id" ? "_id" : D.name];

                result.*D.pointer = std::move(MongoSerializeHelpers<document::element>::handleTypeDeserialization<return_t>(element));
            });

        return std::move(result);
    }

    /** Below is to mongo implementation */
    template <class T>
    bsoncxx::builder::basic::document toMongoDocument(T object);

    struct ToMongoHelpers
    {
        template <class TItem>
        struct is_primitive
        {
            static constexpr bool value =
                std::is_same<TItem, std::string>::value ||
                std::is_same<TItem, std::int32_t>::value ||
                std::is_same<TItem, std::int64_t>::value ||
                std::is_same<TItem, int>::value ||
                std::is_same<TItem, double>::value ||
                std::is_same<TItem, app::shared::Datetime>::value;
        };

        template <class T>
        static void serializeScalar(bsoncxx::builder::basic::document &document, const std::string &name, T value)
        {
            if constexpr (is_optional<T>::value)
            {
                if (value.has_value())
                {
                    document.append(bsoncxx::builder::basic::kvp(name, std::move(value.value())));
                }
                else
                {
                    document.append(bsoncxx::builder::basic::kvp(name, bsoncxx::types::b_null{}));
                }
            }
            else
            {
                document.append(bsoncxx::builder::basic::kvp(name, std::move(value)));
            }
        }

        template <>
        void serializeScalar(bsoncxx::builder::basic::document &document, const std::string &name, shared::Datetime value)
        {
            document.append(bsoncxx::builder::basic::kvp(name, bsoncxx::types::b_date(value)));
        }
        template <>
        void serializeScalar(bsoncxx::builder::basic::document &document, const std::string &name, std::optional<shared::Datetime> value)
        {
            if (value.has_value())
            {
                serializeScalar(document, name, value.value());
            }
            else
            {
                document.append(bsoncxx::builder::basic::kvp(name, bsoncxx::types::b_null{}));
            }
        }

        template <class T>
        static void serializeVector(bsoncxx::builder::basic::document &document, const std::string &name, T value)
        {
            bsoncxx::builder::basic::array ar;

            for (auto &elem : value)
            {
                if constexpr (is_primitive<T>::value)
                {
                    ar.append(std::move(elem));
                }
                else if constexpr (std::is_class<T>::value)
                {
                    ar.append(std::move(toMongoDocument<typename T::value_type>(std::move(elem))));
                }
            }

            document.append(bsoncxx::builder::basic::kvp(name, std::move(ar)));
        }

        template <class T>
        static void serializeObject(bsoncxx::builder::basic::document &document, const std::string &name, T value)
        {
            document.append(bsoncxx::builder::basic::kvp(name, toMongoDocument(std::move(value))));
        }

        template <class TItem>
        static constexpr void handleTypesToMongoDocument(bsoncxx::builder::basic::document &document, const std::string &name, TItem item)
        {
            using real_t = typename unwrap_optional<TItem>::type;

            if constexpr (is_primitive<real_t>::value)
            {
                serializeScalar(document, name, std::move(item));
                return;
            }
            else if constexpr (is_vector<TItem>::value)
            {
                serializeVector(document, name, std::move(item));
                return;
            }
            else if constexpr (std::is_class<TItem>::value)
            {
                serializeObject(document, name, std::move(item));
                return;
            }

            assert((void("Uncovered type"), 0));
        }
    };

    template <class T>
    bsoncxx::builder::basic::document toMongoDocument(T object)
    {
        using D1 = boost::describe::describe_members<T, boost::describe::mod_public>;

        bsoncxx::builder::basic::document document;

        boost::mp11::mp_for_each<D1>(
            [&document, &object](auto D)
            {
                using return_t = std::remove_reference<boost::callable_traits::return_type_t<decltype(D.pointer)>>::type;

                ToMongoHelpers::handleTypesToMongoDocument<return_t>(
                    document,
                    D.name == "id" ? "_id" : D.name,
                    std::move(object.*D.pointer));
            });

        return std::move(document);
    }
}