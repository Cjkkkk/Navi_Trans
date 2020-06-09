#include <iostream>
#include <vector>
#include <cstdint>
#include <limits>
#include <cstring>

#include "./types.hpp"


template <typename T, typename U>
void do_encode(std::vector<uint8_t>::iterator it, T o_data, VAR_INT) {
    uint64_t data;
    if (is_zigzag<U>::value) {
        auto bit_len = std::numeric_limits<T>::digits;
        data = (o_data << 1) ^ (o_data >> bit_len);
    } else {
        data = o_data;
    }
    uint8_t bits = data & 0x7f;
    data = data >> 7;
    while (data != 0) {
        *it = bits | 0x80;
        it += 1;
        bits = data & 0x7f;
        data = data >> 7;
    };
    *it = bits;
}

template <typename T, typename U>
T do_decode(std::vector<uint8_t>::iterator it, VAR_INT) {
    uint64_t data = 0;
    int count = 0;
    while ((*it >> 7) == 1) {
        uint64_t x = (uint64_t)(*it & 0x7f) << (count * 7);
        data |= x;
        count += 1;
        it += 1;
    }
    uint64_t x = (uint64_t)(*it & 0x7f) << (count * 7);
    data |= x;
    count += 1;
    it += 1;
    if (is_zigzag<U>::value) {
        return (T)((data >> 1) ^ - (data & 1));
    } else {
        return (T)(data);
    }
}

template <typename T, typename U>
void encode (std::vector<uint8_t>::iterator it, T o_data) {
    do_encode <T,U> (it, o_data, typename wire_type_trait<U>::wire_type());
}

template <typename T, typename U>
T decode (std::vector<uint8_t>::iterator it) {
    return do_decode <T,U> (it, typename wire_type_trait<U>::wire_type());
}
// ------------------

template <typename T, typename U>
size_t do_pack_size(T o_data, BITS_32) {
    return 4;
}

template <typename T, typename U>
size_t do_pack_size(T o_data, BITS_64) {
    return 8;
}

template <typename T, typename U>
size_t do_pack_size(T o_data, VAR_INT) {
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
size_t pack_size(T o_data) {
    return do_pack_size <T,U> (o_data, typename wire_type_trait<U>::wire_type());
}

int main() {
    int32_t data = -300;
    size_t ps = pack_size<int32_t, INT_32>(data);
    std::cout << "pack_size: " << ps << "\n";
    std::vector<uint8_t> byte_array(ps);
    encode<int32_t, INT_32>(byte_array.begin(), data);
    data = decode<int32_t, INT_32>(byte_array.begin());
    std::cout << data << "\n";
    
    data = -300;
    byte_array.clear();
    ps = pack_size<int32_t, SINT_32>(data);
    std::cout << "pack_size: " << ps << "\n";
    byte_array.resize(ps);
    encode<int32_t, SINT_32>(byte_array.begin(), data);
    data = decode<int32_t, SINT_32>(byte_array.begin());
    std::cout << data << "\n";
    
    return 0;
}