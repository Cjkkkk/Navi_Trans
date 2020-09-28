#ifndef PACK_HPP
#define PACK_HPP

#include <exception>
#include <sstream>
#include <limits>
#include <cstring>
#include <iostream>
#include <tuple>
#include "./types.hpp"
#include "./traits.hpp"
#include "./member.hpp"

namespace navi_trans {
    template <typename T>
    uint32_t do_pack(uint8_t* buf, const T& o_data, VAR_INT) {
        uint64_t data;
        if (is_zigzag<T>::value) {
            auto bit_len = std::numeric_limits<T>::digits;
            data = (o_data << 1) ^ (o_data >> bit_len);
        } else {
            data = o_data;
        }
        uint32_t size = 0;
        uint8_t bits = data & 0x7f;
        data = data >> 7;
        while (data != 0) {
            *buf = bits | 0x80;
            buf += 1;
            size += 1;
            bits = data & 0x7f;
            data = data >> 7;
        };
        size += 1;
        *buf = bits;
        return size;
    }

    template <typename T>
    uint32_t do_unpack(const uint8_t* buf, T& o_data, VAR_INT) {
        uint64_t data = 0;
        uint32_t size = 0;
        while (true) {
            uint64_t x = (uint64_t)(*buf & 0x7f) << (size * 7);
            data |= x;
            size += 1;
            if ((*buf >> 7) == 0) break;
            buf += 1;
        }
        if (is_zigzag<T>::value) {
            o_data = (T)((data >> 1) ^ - (data & 1));
        } else {
            o_data = (T)(data);
        }
        return size;
    }


    template <typename T>
    uint32_t do_unpack(const uint8_t* buf, T& o_data, FIXED_LENGTH) {
        uint32_t size = sizeof(T);
        memcpy(&o_data, buf, size);
        return size;
    }

    template <typename T>
    uint32_t do_pack(uint8_t* buf, const T& o_data, FIXED_LENGTH) {
        uint32_t size = sizeof(T);
        memcpy(buf, &o_data, size);
        return size;
    }

    template <typename T>
    uint32_t pack (uint8_t* buf,  const T& o_data) {
        return do_pack <T> (buf, o_data, typename wire_type_trait<T>::wire_type());
    }

    template <typename T>
    uint32_t unpack (const uint8_t* buf, T& o_data) {
        return do_unpack <T> (buf, o_data, typename wire_type_trait<T>::wire_type());
    }

    template <typename T>
    uint32_t do_pack_size( const T& o_data, FIXED_LENGTH) {
        return sizeof(T);
    }

    template <typename T>
    uint32_t do_pack_size( const T& o_data, VAR_INT) {
        uint64_t data;
        if (is_zigzag<T>::value) {
            auto bit_len = std::numeric_limits<T>::digits;
            data = (o_data << 1) ^ (o_data >> bit_len);
        } else {
            data = o_data;
        }
        uint32_t size = 0;
        data = data >> 7;
        while (data != 0) {
            size += 1;
            data = data >> 7;
        };
        size += 1;
        return size;
    }

    template <typename T>
    uint32_t pack_size( const T& o_data) {
        return do_pack_size <T> (o_data, typename wire_type_trait<T>::wire_type());
    }
    
    template <typename T>
    uint32_t generate_key( const uint32_t& field_number) {
        return (field_number << 3) | wire_type_trait<T>::wire_type::wire_type_value;
    }

    template <typename T>
    uint32_t pack_key(uint8_t* buf,  const uint32_t& field_number) {
        return pack(buf, generate_key<T>(field_number));
    }

    template <typename T>
    uint32_t unpack_key(const uint8_t* buf, uint32_t& field_number) {
        uint32_t data;
        uint8_t wire_type_value;
        uint32_t size = unpack(buf, data);
        field_number = data >> 3;
        wire_type_value = data & 0x7;
        if (wire_type_value != wire_type_trait<T>::wire_type::wire_type_value) {
            std::stringstream fmt;
            fmt << "can not match wire type when unpack, expect " 
                << +wire_type_trait<T>::wire_type::wire_type_value
                << ", got "
                << +wire_type_value;
            std::cout << fmt.str() << "\n";
            throw fmt;
        }
        return size;
    }


    template <typename T, int index = 0, typename _ = void>
    struct helper { 
        using m = typename std::tuple_element<index, typename MemberListTraits<T>::m_list::members>::type;

        static inline uint32_t do_pack(uint8_t* buf, const T& data) { 
            uint32_t payload_size = pack_key<typename m::type>(buf, index);
            payload_size += pack(buf + payload_size, m::const_resolve(data));
            payload_size += helper<T, index + 1>::do_pack(buf + payload_size, data);
            return payload_size;
        }

        static inline uint32_t do_unpack(const uint8_t* buf, T& data, uint32_t payload_size) { 
            uint32_t field_number = 0;
            uint32_t offset = 0;
            if (offset < payload_size) {
                offset = unpack_key<typename m::type>(buf, field_number);
                offset += unpack(buf + offset, m::resolve(data));
                offset += helper<T, index + 1>::do_unpack(buf + offset, data, payload_size - offset);
            }
            return offset;
        }

        static inline uint32_t do_pack_size( const T& data) { 
            uint32_t payload_size = pack_size(generate_key<typename m::type>(index));
            payload_size += pack_size(m::const_resolve(data));
            payload_size += helper<T, index + 1>::do_pack_size(data);
            return payload_size;
        }
    }; 
    

    template <typename T>
    struct helper<T, MemberListTraits<T>::m_list::Count - 1, void>{
        const static int index = MemberListTraits<T>::m_list::Count - 1;
        using m = typename std::tuple_element<index, typename MemberListTraits<T>::m_list::members>::type; 
        
        static inline uint32_t do_pack(uint8_t* buf, const T& data) {
            uint32_t payload_size = pack_key<typename m::type>(buf, index);
            payload_size += pack(buf + payload_size, m::const_resolve(data));
            return payload_size;
        }

        static inline uint32_t do_unpack(const uint8_t* buf, T& data, uint32_t payload_size) { 
            uint32_t field_number = 0;
            uint32_t offset = 0;
            if (offset < payload_size) {
                offset = unpack_key<typename m::type>(buf, field_number);
                offset += unpack(buf + offset, m::resolve(data));
            }
            return offset;
        }

        static inline uint32_t do_pack_size( const T& data) { 
            uint32_t payload_size = pack_size(generate_key<typename m::type>(index));
            payload_size += pack_size(m::const_resolve(data));
            return payload_size;
        }

    }; 

    template <typename T>
    uint32_t do_pack(uint8_t* buf, const T& data, LENGTH_DELIMITED) {
        uint32_t payload_size = helper<T>::do_pack(buf + 4, data);
        do_pack(buf, payload_size, FIXED_LENGTH());
        return payload_size + 4;
    } 

    template <typename T>
    uint32_t do_unpack(const uint8_t* buf, T& data, LENGTH_DELIMITED) { 
        uint32_t payload_size;
        do_unpack(buf, payload_size, FIXED_LENGTH());
        uint32_t offset = helper<T>::do_unpack(buf + 4, data, payload_size);
        if (payload_size != offset) {
            std::stringstream fmt;
            fmt << "expect " 
                << payload_size
                << " bytes of payload, got "
                << offset
                << " bytes of payload";
            std::cout << fmt.str() << "\n";
            throw fmt;
        }
        return payload_size + 4;
    } 

    template <typename T>
    uint32_t do_pack_size( const T& data, LENGTH_DELIMITED) { 
        uint32_t payload_size = helper<T>::do_pack_size(data);
        return payload_size + 4;
    } 

}

#endif