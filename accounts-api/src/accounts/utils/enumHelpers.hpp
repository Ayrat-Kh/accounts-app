#pragma once

#include <boost/describe.hpp>

namespace accounts::utils
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

    template <class E>
    E cStrToEnum(char const *str)
    {
        E result;

        boost::mp11::mp_for_each<boost::describe::describe_enumerators<E>>(
            [&str, &result](auto D)
            {
                if (strcmp(str, D.value) == 0)
                {
                    result = D.name;
                }
            });

        return result;
    }
}