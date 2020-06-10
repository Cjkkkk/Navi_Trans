#include <iostream>
#include <vector>
#include <cstdint>
#include <limits>
#include <cstring>

#include "./types.hpp"


template <typename T, typename U>
size_t do_encode(uint8_t* buf, T* o_data, VAR_INT) {
    uint64_t data;
    if (is_zigzag<U>::value) {
        auto bit_len = std::numeric_limits<T>::digits;
        data = (*o_data << 1) ^ (*o_data >> bit_len);
    } else {
        data = *o_data;
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
size_t do_decode(uint8_t* buf, T* o_data, VAR_INT) {
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
        *o_data = (T)((data >> 1) ^ - (data & 1));
    } else {
        *o_data = (T)(data);
    }
    return size;
}


template <typename T, typename U>
size_t do_encode(uint8_t* buf, T* o_data, BITS_32) {
    memcpy(buf, o_data, 4);
    return 4;
}

template <typename T, typename U>
size_t do_decode(uint8_t* buf, T* o_data, BITS_32) {
    memcpy(o_data, buf, 4);
    return 4;
}

template <typename T, typename U>
size_t do_encode(uint8_t* buf, T* o_data, BITS_64) {
    memcpy(buf, o_data, 8);
    return 8;
}

template <typename T, typename U>
size_t do_decode(uint8_t* buf, T* o_data, BITS_64) {
    memcpy(o_data, buf, 8);
    return 8;
}

template <typename T, typename U>
size_t do_encode(uint8_t* buf, T* o_data, LENGTH_DELIMITED) {
    return o_data->do_encode(buf);
}

template <typename T, typename U>
size_t do_decode(uint8_t* buf, T* o_data, LENGTH_DELIMITED) {
    // o_data = new typename std::remove_pointer<T>::type;
    size_t s = o_data->do_decode(buf);
    return s;
}

template <typename T, typename U>
size_t encode (uint8_t* buf, T* o_data) {
    return do_encode <T,U> (buf, o_data, typename wire_type_trait<U>::wire_type());
}

template <typename T, typename U>
size_t decode (uint8_t* buf, T* o_data) {
    return do_decode <T,U> (buf, o_data, typename wire_type_trait<U>::wire_type());
}
// ------------------
template <typename T, typename U>
size_t do_pack_size(T* o_data, LENGTH_DELIMITED) {
    return o_data->do_pack_size();
}

template <typename T, typename U>
size_t do_pack_size(T* o_data, BITS_32) {
    return 4;
}

template <typename T, typename U>
size_t do_pack_size(T* o_data, BITS_64) {
    return 8;
}

template <typename T, typename U>
size_t do_pack_size(T* o_data, VAR_INT) {
    uint64_t data;
    if (is_zigzag<U>::value) {
        auto bit_len = std::numeric_limits<T>::digits;
        data = (*o_data << 1) ^ (*o_data >> bit_len);
    } else {
        data = *o_data;
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
size_t pack_size(T* o_data) {
    return do_pack_size <T,U> (o_data, typename wire_type_trait<U>::wire_type());
}

class A {
    public:
        int32_t a;
        int32_t b;
        int32_t c;
        A() {}
        size_t do_pack_size() {
            size_t body_size = pack_size<int32_t, INT_32>(&a) + pack_size<int32_t, INT_32>(&b) + pack_size<int32_t, INT_32>(&c);
            size_t head_size = pack_size<size_t, INT_32>(&body_size);
            return head_size + body_size;
        }

        size_t do_encode(uint8_t* buf) {
            size_t body_size = encode<int32_t, INT_32>(buf, &a);
            body_size += encode<int32_t, INT_32>(buf + body_size, &b);
            body_size += encode<int32_t, INT_32>(buf + body_size, &c);
            size_t head_size = encode<size_t, INT_32>(buf + body_size, &body_size);
            return head_size + body_size;
        }

        size_t do_decode(uint8_t* buf) {
            size_t body_size = decode<int32_t, INT_32>(buf, &a);
            body_size += decode<int32_t, INT_32>(buf + body_size, &b);
            body_size += decode<int32_t, INT_32>(buf + body_size, &c);
            size_t head_size = decode<size_t, INT_32>(buf + body_size, &body_size);
            return head_size + body_size;
        }
};

class B {
    public:
        A a;
        int32_t b;
        int32_t c;
        B() {}
        size_t do_pack_size() {
            size_t body_size = pack_size<A, EMBEDDED_MESSAGES>(&a) + pack_size<int32_t, INT_32>(&b) + pack_size<int32_t, INT_32>(&c);
            size_t head_size = pack_size<size_t, INT_32>(&body_size);
            return head_size + body_size;
        }

        size_t do_encode(uint8_t* buf) {
            size_t body_size = encode<A, EMBEDDED_MESSAGES>(buf, &a);
            body_size += encode<int32_t, INT_32>(buf + body_size, &b);
            body_size += encode<int32_t, INT_32>(buf + body_size, &c);
            size_t head_size = encode<size_t, INT_32>(buf + body_size, &body_size);
            return head_size + body_size;
        }

        size_t do_decode(uint8_t* buf) {
            size_t body_size = decode<A, EMBEDDED_MESSAGES>(buf, &a);
            body_size += decode<int32_t, INT_32>(buf + body_size, &b);
            body_size += decode<int32_t, INT_32>(buf + body_size, &c);
            size_t head_size = decode<size_t, INT_32>(buf + body_size, &body_size);
            return head_size + body_size;
        }
};

int main() {
    int32_t data = -300;
    size_t ps = pack_size<int32_t, INT_32>(&data);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf = new uint8_t[ps];
    encode<int32_t, INT_32>(buf, &data);
    decode<int32_t, INT_32>(buf, &data);
    std::cout << data << "\n";
    delete[] buf;

    int32_t data1 = -320;
    ps = pack_size<int32_t, SINT_32>(&data1);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf1 = new uint8_t[ps];
    encode<int32_t, SINT_32>(buf1, &data1);
    decode<int32_t, SINT_32>(buf1, &data1);
    std::cout << data1 << "\n";
    delete[] buf1;

    float data2 = 3.2;
    ps = pack_size<float, FLOAT>(&data2);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf2 = new uint8_t[ps];
    encode<float, FLOAT>(buf2, &data2);
    decode<float, FLOAT>(buf2, &data2);
    std::cout << data2 << "\n";
    delete[] buf2;

    double data3 = 3.4;
    ps = pack_size<double, DOUBLE>(&data3);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf3 = new uint8_t[ps];
    encode<double, DOUBLE>(buf3, &data3);
    decode<double, DOUBLE>(buf3, &data3);
    std::cout << data3 << "\n";
    delete[] buf3;

    // message
    A data4;
    data4.a = 10;
    data4.b = 15;
    data4.c = 20;
    ps = pack_size<A, EMBEDDED_MESSAGES>(&data4);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf4 = new uint8_t[ps];
    encode<A, EMBEDDED_MESSAGES>(buf4, &data4);
    // decode
    decode<A, EMBEDDED_MESSAGES>(buf4, &data4);
    std::cout << data4.a << "\n";
    std::cout << data4.b << "\n";
    std::cout << data4.c << "\n";
    delete[] buf4;

    // nested message
    B data5;
    A a;
    a.a = 5;
    a.b = 20;
    a.c = 30;
    data5.a = a;
    data5.b = 15;
    data5.c = 20;
    ps = pack_size<B, EMBEDDED_MESSAGES>(&data5);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf5 = new uint8_t[ps];
    encode<B, EMBEDDED_MESSAGES>(buf5, &data5);
    // decode
    decode<B, EMBEDDED_MESSAGES>(buf5, &data5);
    std::cout << data5.a.a << "\n";
    std::cout << data5.a.b << "\n";
    std::cout << data5.a.c << "\n";
    std::cout << data5.b << "\n";
    std::cout << data5.c << "\n";
    delete[] buf5;

    return 0;
}