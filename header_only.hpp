#ifndef PACK_HPP
#define PACK_HPP

#include <exception>
#include <sstream>
#include <limits>
#include <vector>
#include <cstring>
#include <iostream>

#include "./pack.hpp"
#include "./types.hpp"
#include "./traits.hpp"


namespace navi_trans {
    template <typename T, typename M, typename ...Ms>
    inline void do_pack_member(uint8_t* buf, const T& o_data, size_t& payload_size, size_t field_number) {
        payload_size += pack_key<INT_32>(buf + payload_size, field_number);
        payload_size += pack<INT_32>(buf + payload_size, b);
        do_pack_member<T, Ms...>(buf, o_data, payload_size, field_number + 1);
    }

    template <typename T, typename M>
    inline void do_pack_member(uint8_t* buf, const T& o_data, size_t& payload_size, size_t field_number) {
        payload_size += pack_key<INT_32>(buf + payload_size, field_number);
        payload_size += pack<INT_32>(buf + payload_size, b);
    }

    template <typename T, typename ...members>
    size_t do_pack(uint8_t* buf, const T& o_data, LENGTH_DELIMITED) {
        size_t payload_size = 4;
        do_pack_member<T, members...>(buf, o_data, payload_size, 0);
        do_pack<FIXED_32>(buf, payload_size, BITS_32());
        return payload_size;
    }

    template <typename T, typename M, typename ...Ms>
    inline void do_unpack_member(uint8_t* buf, const T& o_data, size_t& offset, size_t field_number) {
        offset += unpack_key<INT_32>(buf + offset, field_number);
        offset += unpack<INT_32>(buf + offset, a);
        do_unpack_member<T, Ms...>(buf, o_data, offset, field_number + 1);
    }

    template <typename T, typename M>
    inline void do_unpack_member(uint8_t* buf, const T& o_data, size_t& offset, size_t field_number) {
        offset += unpack_key<INT_32>(buf + offset, field_number);
        offset += unpack<INT_32>(buf + offset, a);
    }

    template <typename T, typename ...members>
    size_t do_unpack(uint8_t* buf, const T& o_data, LENGTH_DELIMITED) {
        size_t payload_size;
        do_unpack<FIXED_32>(buf, payload_size, BITS_32());
        uint32_t field_number = 0;
        size_t offset = 4;
        do_unpack_member<T, members...>(buf, o_data, offset, field_number);
        return payload_size + 4;
    }
}

#endif