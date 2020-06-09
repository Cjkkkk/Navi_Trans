#include <iostream>
#include <vector>
#include <cstdint>
#include <limits>
#include "./types.hpp"


template <typename T, typename U>
void do_encode(std::vector<uint8_t>& byte_array, T o_data, VAR_INT) {
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
        byte_array.push_back(bits | 0x80);
        bits = data & 0x7f;
        data = data >> 7;
    };
    byte_array.push_back(bits);
}

template <typename T, typename U>
T do_decode(std::vector<uint8_t>& byte_array, VAR_INT) {
    uint64_t data = 0;
    int count = 0;
    for (auto byte : byte_array) {
        uint64_t x = (uint64_t)(byte & 0x7f) << (count * 7);
        data |= x;
        count += 1;
    }
    if (is_zigzag<U>::value) {
        return (T)((data >> 1) ^ - (data & 1));
    } else {
        return (T)(data);
    }
}


template <typename T, typename U>
void encode (std::vector<uint8_t>& byte_array, T o_data) {
    do_encode <T,U> (byte_array, o_data, typename wire_type_trait<U>::wire_type());
}

template <typename T, typename U>
T decode (std::vector<uint8_t>& byte_array) {
    return do_decode <T,U> (byte_array, typename wire_type_trait<U>::wire_type());
}


int main() {
    std::vector<uint8_t> byte_array;
    encode<int32_t, INT_32>(byte_array, -300);
    std::cout << "length: " << byte_array.size() << "\n";
    int32_t data = decode<int32_t, INT_32>(byte_array);
    std::cout << data << "\n";

    byte_array.clear();
    encode<int32_t, SINT_32>(byte_array, -300);
    std::cout << "length: " << byte_array.size() << "\n";
    data = decode<int32_t, SINT_32>(byte_array);
    std::cout << data << "\n";

    return 0;
}