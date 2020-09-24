#ifndef TRAITS_HPP
#define TRAITS_HPP

#include "./types.hpp"

namespace navi_trans {
// define help traits
    template <typename T>
    struct is_zigzag {
        static const bool value = false;
    };

    template <>
    struct is_zigzag<SINT_32> {
        static const bool value = true;
    };

    template <>
    struct is_zigzag<SINT_64> {
        static const bool value = true;
    };


    template <typename T>
    struct wire_type_trait {
        typedef typename T::wire_type wire_type;
    };
}
#endif