#ifndef PACK_HPP
#define PACK_HPP

#include "./types.hpp"

namespace navi_trans {
    template <typename T, typename U>
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

    template <typename T, typename U>
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


    template <typename T, typename U>
    size_t do_pack(uint8_t* buf, const T& o_data, BITS_32) {
        memcpy(buf, &o_data, 4);
        return 4;
    }

    template <typename T, typename U>
    size_t do_unpack(uint8_t* buf, T& o_data, BITS_32) {
        memcpy(&o_data, buf, 4);
        return 4;
    }

    template <typename T, typename U>
    size_t do_pack(uint8_t* buf, const T& o_data, BITS_64) {
        memcpy(buf, &o_data, 8);
        return 8;
    }

    template <typename T, typename U>
    size_t do_unpack(uint8_t* buf, T& o_data, BITS_64) {
        memcpy(&o_data, buf, 8);
        return 8;
    }

    template <typename T, typename U>
    size_t do_pack(uint8_t* buf, const T& o_data, LENGTH_DELIMITED) {
        return o_data.do_pack(buf);
    }

    template <typename T, typename U>
    size_t do_unpack(uint8_t* buf, T& o_data, LENGTH_DELIMITED) {
        // o_data = new typename std::remove_pointer<T>::type;
        size_t s = o_data.do_unpack(buf);
        return s;
    }

    template <typename T, typename U>
    size_t pack (uint8_t* buf, const T& o_data) {
        return do_pack <T,U> (buf, o_data, typename wire_type_trait<U>::wire_type());
    }

    template <typename T, typename U>
    size_t unpack (uint8_t* buf, T& o_data) {
        return do_unpack <T,U> (buf, o_data, typename wire_type_trait<U>::wire_type());
    }
    // ------------------
    template <typename T, typename U>
    size_t do_pack_size(const T& o_data, LENGTH_DELIMITED) {
        return o_data.do_pack_size();
    }

    template <typename T, typename U>
    size_t do_pack_size(const T& o_data, BITS_32) {
        return 4;
    }

    template <typename T, typename U>
    size_t do_pack_size(const T& o_data, BITS_64) {
        return 8;
    }

    template <typename T, typename U>
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

    template <typename T, typename U>
    size_t pack_size(const T& o_data) {
        return do_pack_size <T,U> (o_data, typename wire_type_trait<U>::wire_type());
    }

    template <typename T, typename U, size_t...>
    class Iteratorable{};

    template <typename T, typename U, size_t N>
    class Iteratorable<T, U, N>{
        public:
            T data[N];
            Iteratorable() {}
            size_t do_pack_size() const {
                size_t payload_size = 0;
                for (auto iter = std::begin(data); iter != std::end(data); ++iter) {
                    payload_size += pack_size<T, U>(*iter);
                }
                size_t head_size = pack_size<size_t, INT_32>(payload_size);
                return head_size + payload_size;
            }

            size_t do_pack(uint8_t* buf) const {
                size_t payload_size = 0;
                for (auto iter = std::begin(data); iter != std::end(data); ++iter) {
                    payload_size += pack<T, U>(buf + payload_size, *iter);
                }
                size_t head_size = pack<size_t, INT_32>(buf + payload_size, payload_size);
                return head_size + payload_size;
            }

            size_t do_unpack(uint8_t* buf) {
                size_t payload_size = 0;
                for (auto iter = std::begin(data); iter != std::end(data); ++iter) {
                    payload_size += unpack<T, U>(buf + payload_size, *iter);
                }
                size_t head_size = unpack<size_t, INT_32>(buf + payload_size, payload_size);
                return head_size + payload_size;
            }
    };

    template <typename T>
    uint32_t generate_key(uint32_t field_number) {
        return (field_number << 3) | wire_type_trait<T>::wire_type::wire_type_value;
    }

    // TODO: add key information into serialization
    // message len | k | v | k | v ...
    // array len | v | v | v ...
    // current message v | v | v | len
    // current array v | v | v | len
}

#endif