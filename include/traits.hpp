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
    struct is_zigzag<int16_t> {
        static const bool value = true;
    };

    template <>
    struct is_zigzag<int8_t> {
        static const bool value = true;
    };

    template <typename T, typename _ = void>
    struct wire_type_trait {
        using wire_type = LENGTH_DELIMITED;
    };

    // for enum
    template <typename T>
    struct wire_type_trait<T, typename std::enable_if<std::is_enum<T>::value>::type> { \
        using wire_type = VAR_INT;\
    };

    #define ENTRY(t) \
    template <>  \
    struct wire_type_trait<t> { \
        using wire_type = VAR_INT;\
    };
        List_Of_VAR_INT_Types
    #undef ENTRY

    #define ENTRY(t) \
    template <>  \
    struct wire_type_trait<t> { \
        using wire_type = FIXED_LENGTH;\
    };
        List_Of_FIXED_LENGTH_Types
    #undef ENTRY

    template <typename T>
    struct MemberListTraits; 
}
#endif