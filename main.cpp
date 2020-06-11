#include <iostream>
#include <vector>
#include <cstdint>
#include <limits>
#include <cstring>

#include "./types.hpp"


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
            size_t body_size = 0;
            for (auto iter = std::begin(data); iter != std::end(data); ++iter) {
                body_size += pack_size<T, U>(*iter);
            }
            size_t head_size = pack_size<size_t, INT_32>(body_size);
            return head_size + body_size;
        }

        size_t do_pack(uint8_t* buf) const {
            size_t body_size = 0;
            for (auto iter = std::begin(data); iter != std::end(data); ++iter) {
                body_size += pack<T, U>(buf + body_size, *iter);
            }
            size_t head_size = pack<size_t, INT_32>(buf + body_size, body_size);
            return head_size + body_size;
        }

        size_t do_unpack(uint8_t* buf) {
            size_t body_size = 0;
            for (auto iter = std::begin(data); iter != std::end(data); ++iter) {
                body_size += unpack<T, U>(buf + body_size, *iter);
            }
            size_t head_size = unpack<size_t, INT_32>(buf + body_size, body_size);
            return head_size + body_size;
        }
};

class A {
    public:
        int32_t a;
        int32_t b;
        int32_t c;
        A() {}
        size_t do_pack_size() const {
            size_t body_size = pack_size<int32_t, INT_32>(a) + pack_size<int32_t, INT_32>(b) + pack_size<int32_t, INT_32>(c);
            size_t head_size = pack_size<size_t, INT_32>(body_size);
            return head_size + body_size;
        }

        size_t do_pack(uint8_t* buf) const {
            size_t body_size = pack<int32_t, INT_32>(buf, a);
            body_size += pack<int32_t, INT_32>(buf + body_size, b);
            body_size += pack<int32_t, INT_32>(buf + body_size, c);
            size_t head_size = pack<size_t, INT_32>(buf + body_size, body_size);
            return head_size + body_size;
        }

        size_t do_unpack(uint8_t* buf) {
            size_t body_size = unpack<int32_t, INT_32>(buf, a);
            body_size += unpack<int32_t, INT_32>(buf + body_size, b);
            body_size += unpack<int32_t, INT_32>(buf + body_size, c);
            size_t head_size = unpack<size_t, INT_32>(buf + body_size, body_size);
            return head_size + body_size;
        }
};

class B {
    public:
        A a;
        int32_t b;
        int32_t c;
        B() {}
        size_t do_pack_size() const {
            size_t body_size = pack_size<A, EMBEDDED_MESSAGES>(a) + pack_size<int32_t, INT_32>(b) + pack_size<int32_t, INT_32>(c);
            size_t head_size = pack_size<size_t, INT_32>(body_size);
            return head_size + body_size;
        }

        size_t do_pack(uint8_t* buf) const {
            size_t body_size = pack<A, EMBEDDED_MESSAGES>(buf, a);
            body_size += pack<int32_t, INT_32>(buf + body_size, b);
            body_size += pack<int32_t, INT_32>(buf + body_size, c);
            size_t head_size = pack<size_t, INT_32>(buf + body_size, body_size);
            return head_size + body_size;
        }

        size_t do_unpack(uint8_t* buf) {
            size_t body_size = unpack<A, EMBEDDED_MESSAGES>(buf, a);
            body_size += unpack<int32_t, INT_32>(buf + body_size, b);
            body_size += unpack<int32_t, INT_32>(buf + body_size, c);
            size_t head_size = unpack<size_t, INT_32>(buf + body_size, body_size);
            return head_size + body_size;
        }
};

class C {
    public:
        Iteratorable<int32_t, INT_32, 4> a;
        int32_t b;
        C() {}
        size_t do_pack_size() const {
            size_t body_size = pack_size<Iteratorable<int32_t, INT_32, 4>, ARRAY>(a) + pack_size<int32_t, INT_32>(b);
            size_t head_size = pack_size<size_t, INT_32>(body_size);
            return head_size + body_size;
        }

        size_t do_pack(uint8_t* buf) const {
            size_t body_size = pack<Iteratorable<int32_t, INT_32, 4>, ARRAY>(buf, a);
            body_size += pack<int32_t, INT_32>(buf + body_size, b);
            size_t head_size = pack<size_t, INT_32>(buf + body_size, body_size);
            return head_size + body_size;
        }

        size_t do_unpack(uint8_t* buf) {
            size_t body_size = unpack<Iteratorable<int32_t, INT_32, 4>, ARRAY>(buf, a);
            body_size += unpack<int32_t, INT_32>(buf + body_size, b);
            size_t head_size = unpack<size_t, INT_32>(buf + body_size, body_size);
            return head_size + body_size;
        }
    
};

int main() {
    int32_t data = -300;
    size_t ps = pack_size<int32_t, INT_32>(data);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf = new uint8_t[ps];
    pack<int32_t, INT_32>(buf, data);
    unpack<int32_t, INT_32>(buf, data);
    std::cout << data << "\n";
    delete[] buf;

    int32_t data1 = -320;
    ps = pack_size<int32_t, SINT_32>(data1);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf1 = new uint8_t[ps];
    pack<int32_t, SINT_32>(buf1, data1);
    unpack<int32_t, SINT_32>(buf1, data1);
    std::cout << data1 << "\n";
    delete[] buf1;

    float data2 = 3.2;
    ps = pack_size<float, FLOAT>(data2);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf2 = new uint8_t[ps];
    pack<float, FLOAT>(buf2, data2);
    unpack<float, FLOAT>(buf2, data2);
    std::cout << data2 << "\n";
    delete[] buf2;

    double data3 = 3.4;
    ps = pack_size<double, DOUBLE>(data3);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf3 = new uint8_t[ps];
    pack<double, DOUBLE>(buf3, data3);
    unpack<double, DOUBLE>(buf3, data3);
    std::cout << data3 << "\n";
    delete[] buf3;

    // message
    A data4;
    data4.a = 10;
    data4.b = 15;
    data4.c = 20;
    ps = pack_size<A, EMBEDDED_MESSAGES>(data4);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf4 = new uint8_t[ps];
    pack<A, EMBEDDED_MESSAGES>(buf4, data4);
    // unpack
    unpack<A, EMBEDDED_MESSAGES>(buf4, data4);
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
    ps = pack_size<B, EMBEDDED_MESSAGES>(data5);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf5 = new uint8_t[ps];
    pack<B, EMBEDDED_MESSAGES>(buf5, data5);
    // unpack
    unpack<B, EMBEDDED_MESSAGES>(buf5, data5);
    std::cout << data5.a.a << "\n";
    std::cout << data5.a.b << "\n";
    std::cout << data5.a.c << "\n";
    std::cout << data5.b << "\n";
    std::cout << data5.c << "\n";
    delete[] buf5;

    // message with array
    C data6;
    data6.a.data[0] = 5;
    data6.a.data[1] = 10;
    data6.a.data[2] = 15;
    data6.a.data[3] = 20;
    data6.b = 40;
    ps = pack_size<C, EMBEDDED_MESSAGES>(data6);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf6 = new uint8_t[ps];
    pack<C, EMBEDDED_MESSAGES>(buf6, data6);
    // unpack
    unpack<C, EMBEDDED_MESSAGES>(buf6, data6);
    std::cout << data6.a.data[0] << "\n";
    std::cout << data6.a.data[1] << "\n";
    std::cout << data6.a.data[2] << "\n";
    std::cout << data6.a.data[3] << "\n";
    std::cout << data6.b << "\n";
    delete[] buf6;

    return 0;
}