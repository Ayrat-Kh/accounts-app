#pragma once

#include <boost/describe.hpp>

namespace accounts
{
    template <class E>
    char const *enumToString(E e)
    {
        char const *r = "(unnamed)";

        boost::mp11::mp_for_each<boost::describe::describe_enumerators<E>>(
            [&](auto D)
            {
                if (e == D.value)
                {
                    r = D.name;
                }
            });

        return r;
    }

    template <class TEnum>
    TEnum cStrToEnum(const char *str)
    {
        TEnum result;

        boost::mp11::mp_for_each<boost::describe::describe_enumerators<TEnum>>(
            [&str, &result](auto D)
            {
                if (strcmp(str, D.name) == 0)
                {
                    result = D.value;
                }
            });

        return result;
    }
}