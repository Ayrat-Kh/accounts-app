#pragma once

#include <boost/json.hpp>
#include <boost/describe.hpp>

#include "accounts/shared/types.hpp"

namespace accounts
{
    template <
        class T,
        class D1 = boost::describe::describe_members<
            T,
            boost::describe::mod_public | boost::describe::mod_protected>,
        class D2 = boost::describe::describe_members<T, boost::describe::mod_private>,
        class En = std::enable_if_t<boost::mp11::mp_empty<D2>::value>>

    void tag_invoke(boost::json::value_from_tag const &, boost::json::value &v, T const &t)
    {
        auto &obj = v.emplace_object();

        boost::mp11::mp_for_each<D1>(
            [&](auto D)
            { obj[D.name] = boost::json::value_from(t.*D.pointer); });
    }

    template <class T>
    void extract(boost::json::object const &obj, char const *name, T &value)
    {
        value = boost::json::value_to<T>(obj.at(name));
    }

    template <class T,
              class D1 = boost::describe::describe_members<
                  T,
                  boost::describe::mod_public | boost::describe::mod_protected>,
              class D2 = boost::describe::describe_members<T, boost::describe::mod_private>,
              class En = std::enable_if_t<boost::mp11::mp_empty<D2>::value>>

    T tag_invoke(boost::json::value_to_tag<T> const &, boost::json::value const &v)
    {
        auto const &obj = v.as_object();

        T t{};

        boost::mp11::mp_for_each<D1>([&](auto D)
                                     { extract(obj, D.name, t.*D.pointer); });

        return t;
    }
}

namespace std::chrono
{
    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ::accounts::Datetime timePoint);
    boost::json::result<::accounts::Datetime> tag_invoke(boost::json::try_value_to_tag<::accounts::Datetime> v, const boost::json::value &s);
}
