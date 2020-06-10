#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>

#define List_Of_VAR_INT_Types \
    ENTRY(INT_32, 0) \
    ENTRY(INT_64, 1)\
    ENTRY(UINT_32, 2)\
    ENTRY(UINT_64, 3) \
    ENTRY(SINT_32, 4)\
    ENTRY(SINT_64, 5)\
    ENTRY(BOOL, 6)\
    ENTRY(ENUM, 7)

#define List_Of_BITS_64_Types \
    ENTRY(FIXED_64, 8)\
    ENTRY(SFIXED_64, 9)\
    ENTRY(DOUBLE, 10)

#define List_Of_LENGTH_DELIMITED_Types \
    ENTRY(BYTES, 11)\
    ENTRY(STRING, 12)\
    ENTRY(EMBEDDED_MESSAGES, 13)\
    ENTRY(PACKED_REPEATED_FIELDS, 14)

#define List_Of_BITS_32_Types \
    ENTRY(FIXED_32, 15)\
    ENTRY(SFIXED_32, 16)\
    ENTRY(FLOAT, 17)

#define List_Of_Origin_Types \
    List_Of_VAR_INT_Types \
    List_Of_BITS_64_Types \
    List_Of_BITS_32_Types \
    List_Of_LENGTH_DELIMITED_Types

#define List_Of_Wire_Types \
    ENTRY(VAR_INT, 0) \
    ENTRY(BITS_64, 1) \
    ENTRY(LENGTH_DELIMITED, 2)\
    ENTRY(BITS_32, 5)

// define struct
#define ENTRY(t, v) struct t { static const uint8_t wire_type_value = v;}; 
    List_Of_Wire_Types
#undef ENTRY

#define ENTRY(t, v) struct t { \
    typedef VAR_INT wire_type; \
};
    List_Of_VAR_INT_Types
#undef ENTRY

#define ENTRY(t, v) struct t { \
    typedef BITS_64 wire_type; \
};
    List_Of_BITS_64_Types
#undef ENTRY

#define ENTRY(t, v) struct t { \
    typedef BITS_32 wire_type; \
};
    List_Of_BITS_32_Types
#undef ENTRY

#define ENTRY(t, v) struct t { \
    typedef LENGTH_DELIMITED wire_type; \
};
    List_Of_LENGTH_DELIMITED_Types
#undef ENTRY


#define ENTRY(t, v) std::string as_str(t ty) {return #t;} \
    List_Of_Origin_Types \
    List_Of_Wire_Types
#undef ENTRY

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

#endif