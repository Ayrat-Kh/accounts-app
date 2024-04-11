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
                    r = D.name;
            });

        return r;
    }
}