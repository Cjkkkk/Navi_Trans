#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>

#define List_Of_Wire_Types \
    ENTRY(VAR_INT, 0) \
    ENTRY(BITS_64, 1) \
    ENTRY(LENGTH_DELIMITED, 2)\
    ENTRY(BITS_32, 5)

#define List_Of_VAR_INT_Types \
    ENTRY(int16_t) \
    ENTRY(int8_t) \
    ENTRY(uint16_t)\
    ENTRY(uint8_t) \
    ENTRY(bool) \
    ENTRY(char)

#define List_Of_BITS_64_Types \
    ENTRY(uint64_t)\
    ENTRY(int64_t)\
    ENTRY(double)

#define List_Of_BITS_32_Types \
    ENTRY(uint32_t)\
    ENTRY(int32_t)\
    ENTRY(float)

#define List_Of_Origin_Types \
    List_Of_VAR_INT_Types \
    List_Of_BITS_64_Types \
    List_Of_BITS_32_Types

namespace navi_trans {
    // define struct
    #define ENTRY(t, v) struct t { static const uint8_t wire_type_value = v;}; 
        List_Of_Wire_Types
    #undef ENTRY

    #define ENTRY(t, v) std::string as_str(t ty) {return #t;} \
        List_Of_Origin_Types \
        List_Of_Wire_Types
    #undef ENTRY
}
#endif