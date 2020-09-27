#ifndef STL_HPP
#define STL_HPP

#include "./pack.hpp"
#include <vector>

namespace navi_trans {
    template <typename T>
    inline uint32_t size(T& data) {
        return data.size();
    }

    template <typename T, size_t N>
    inline uint32_t size(T (&data)[N]) {
        return N;
    }

    template <typename T>
    inline void resize(T& data, uint32_t count) {
        data.resize(count);
    }

    template <typename T, size_t N>
    inline void resize(T (&data)[N], uint32_t count) { }

    template <typename T>
    struct helper <
        T, 
        0,
        typename std::enable_if<
            std::is_same<T, std::vector<typename T::value_type, typename T::allocator_type>>::value  
            || std::is_same<T, std::string>::value
            >::type
        > { 
        static inline uint32_t do_pack(uint8_t* buf, const T& data) {
            uint32_t payload_size = 0;
            payload_size += pack(buf + payload_size, size(data));
            for ( auto &v: data) {
                payload_size += pack(buf + payload_size, v);
            }
            return payload_size;
        }

        static inline uint32_t do_unpack(const uint8_t* buf, T& data, uint32_t payload_size) { 
            uint32_t offset = 0;
            uint32_t count = 0;
            offset += unpack(buf + offset, count);
            resize(data, count);
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

    template <typename T>
    struct helper <
        T, 
        0,
        typename std::enable_if<std::is_array<T>::value>::type
        > { 
        static inline uint32_t do_pack(uint8_t* buf, const T& data) {
            uint32_t payload_size = 0;
            payload_size += pack(buf + payload_size, size(data));
            for ( auto &v: data) {
                payload_size += pack(buf + payload_size, v);
            }
            return payload_size;
        }

        static inline uint32_t do_unpack(const uint8_t* buf, T& data, uint32_t payload_size) { 
            uint32_t offset = 0;
            uint32_t count = 0;
            offset += unpack(buf + offset, count);
            resize(data, count);
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