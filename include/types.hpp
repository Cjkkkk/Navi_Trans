#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>

#define List_Of_Wire_Types \
    ENTRY(VAR_INT, 0) \
    ENTRY(FIXED_LENGTH, 1) \
    ENTRY(LENGTH_DELIMITED, 2)\

#define List_Of_VAR_INT_Types \
    

#define List_Of_FIXED_LENGTH_Types \
    ENTRY(bool) \
    ENTRY(char) \
    ENTRY(int8_t) \
    ENTRY(uint8_t) \
    ENTRY(int16_t) \
    ENTRY(uint16_t)\
    ENTRY(int32_t)\
    ENTRY(uint32_t)\
    ENTRY(int64_t)\
    ENTRY(uint64_t)\
    ENTRY(float) \
    ENTRY(double)

#define List_Of_Origin_Types \
    List_Of_VAR_INT_Types \
    List_Of_FIXED_LENGTH_Types \
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