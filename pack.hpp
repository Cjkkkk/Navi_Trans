#ifndef PACK_HPP
#define PACK_HPP

#include <exception>
#include <sstream>
#include <limits>
#include <vector>
#include <cstring>
#include <iostream>
#include "./types.hpp"
#include "./traits.hpp"


namespace navi_trans {
    template <typename U, typename T>
    size_t do_pack(uint8_t* buf, const T& o_data, VAR_INT) {
        uint64_t data;
        if (is_zigzag<U>::value) {
            auto bit_len = std::numeric_limits<T>::digits;
            data = (o_data << 1) ^ (o_data >> bit_len);
        } else {
            data = o_data;
        }
        size_t size = 0;
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

    template <typename U, typename T>
    size_t do_unpack(uint8_t* buf, T& o_data, VAR_INT) {
        uint64_t data = 0;
        size_t size = 0;
        while (true) {
            uint64_t x = (uint64_t)(*buf & 0x7f) << (size * 7);
            data |= x;
            size += 1;
            if ((*buf >> 7) == 0) break;
            buf += 1;
        }
        if (is_zigzag<U>::value) {
            o_data = (T)((data >> 1) ^ - (data & 1));
        } else {
            o_data = (T)(data);
        }
        return size;
    }


    template <typename U, typename T>
    size_t do_pack(uint8_t* buf, const T& o_data, BITS_32) {
        memcpy(buf, &o_data, 4);
        return 4;
    }

    template <typename U, typename T>
    size_t do_unpack(uint8_t* buf, T& o_data, BITS_32) {
        memcpy(&o_data, buf, 4);
        return 4;
    }

    template <typename U, typename T>
    size_t do_pack(uint8_t* buf, const T& o_data, BITS_64) {
        memcpy(buf, &o_data, 8);
        return 8;
    }

    template <typename U, typename T>
    size_t do_unpack(uint8_t* buf, T& o_data, BITS_64) {
        memcpy(&o_data, buf, 8);
        return 8;
    }

    template <typename U, typename T>
    size_t do_pack(uint8_t* buf, const T& o_data, LENGTH_DELIMITED) {
        size_t payload_size = o_data.do_pack(buf + 4);
        do_pack<FIXED_32>(buf, payload_size, BITS_32());
        return payload_size + 4;
    }

    template <typename U, typename T>
    size_t do_unpack(uint8_t* buf, T& o_data, LENGTH_DELIMITED) {
        // o_data = new typename std::remove_pointer<T>::type;
        size_t payload_size;
        do_unpack<FIXED_32>(buf, payload_size, BITS_32());
        o_data.do_unpack(buf + 4, payload_size);
        return payload_size + 4;
    }

    template <typename U, typename T>
    size_t pack (uint8_t* buf, const T& o_data) {
        return do_pack <U, T> (buf, o_data, typename wire_type_trait<U>::wire_type());
    }

    template <typename U, typename T>
    size_t unpack (uint8_t* buf, T& o_data) {
        return do_unpack <U, T> (buf, o_data, typename wire_type_trait<U>::wire_type());
    }
    // ------------------
    template <typename U, typename T>
    size_t do_pack_size(const T& o_data, LENGTH_DELIMITED) {
        return o_data.do_pack_size() + 4;
    }

    template <typename U, typename T>
    size_t do_pack_size(const T& o_data, BITS_32) {
        return 4;
    }

    template <typename U, typename T>
    size_t do_pack_size(const T& o_data, BITS_64) {
        return 8;
    }

    template <typename U, typename T>
    size_t do_pack_size(const T& o_data, VAR_INT) {
        uint64_t data;
        if (is_zigzag<U>::value) {
            auto bit_len = std::numeric_limits<T>::digits;
            data = (o_data << 1) ^ (o_data >> bit_len);
        } else {
            data = o_data;
        }
        size_t size = 0;
        data = data >> 7;
        while (data != 0) {
            size += 1;
            data = data >> 7;
        };
        size += 1;
        return size;
    }

    template <typename U, typename T>
    size_t pack_size(const T& o_data) {
        return do_pack_size <U, T> (o_data, typename wire_type_trait<U>::wire_type());
    }

    template <typename U, typename T, size_t...>
    class Array{};

    template <typename U, typename T>
    class Array<T, U>{
        public:
            std::vector<T> data;
            Array() {}
            size_t do_pack_size() const {
                size_t payload_size = 0;
                for (auto iter = std::begin(data); iter != std::end(data); ++iter) {
                    payload_size += pack_size<U, T>(*iter);
                }
                return payload_size;
            }

            size_t do_pack(uint8_t* buf) const {
                size_t payload_size = 0;
                for (auto iter = std::begin(data); iter != std::end(data); ++iter) {
                    payload_size += pack<U, T>(buf + payload_size, *iter);
                }
                return payload_size;
            }

            size_t do_unpack(uint8_t* buf, size_t& payload_size) {
                size_t offset = 0;
                while(offset < payload_size) {
                    T t = T();
                    offset += unpack<U, T>(buf + offset, t);
                    data.push_back(std::move(t));
                }
                return payload_size;
            }
    };

    template <typename T>
    uint32_t generate_key(const uint32_t& field_number) {
        return (field_number << 3) | wire_type_trait<T>::wire_type::wire_type_value;
    }

    template <typename T>
    size_t pack_key(uint8_t* buf, const uint32_t& field_number) {
        return pack<INT_32>(buf, generate_key<T>(field_number));
    }

    template <typename T>
    size_t unpack_key(uint8_t* buf, uint32_t& field_number) {
        uint32_t data;
        uint8_t wire_type_value;
        size_t size = unpack<INT_32>(buf, data);
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
}

#endif