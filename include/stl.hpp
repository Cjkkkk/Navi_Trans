#ifndef STL_HPP
#define STL_HPP

#include <exception>
#include <sstream>
#include <limits>
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>
#include "./pack.hpp"

namespace navi_trans {
    template <typename T>
    struct helper <
        T, 
        0,
        typename std::enable_if<
            std::is_same<T, std::vector<typename T::value_type, typename T::allocator_type>>::value 
            || std::is_same<T, std::string>::value
            || std::is_array<T>::value
            >::type
        > { 
        static inline uint32_t do_pack(uint8_t* buf, const T& data) {
            uint32_t payload_size = 0;
            for ( auto &v: data) {
                payload_size += pack(buf + payload_size, v);
            }
            return payload_size;
        }

        static inline uint32_t do_unpack(const uint8_t* buf, T& data) { 
            uint32_t offset = 0;
            for ( auto &v: data) {
                offset += unpack(buf + offset, v);
            }
            return offset;
        }

        static inline uint32_t do_pack_size( const T& data) { 
            uint32_t payload_size = 0;
            for ( auto &v: data) {
                payload_size += pack_size(v);
            }
            return payload_size;
        }
    }; 
}
#endif