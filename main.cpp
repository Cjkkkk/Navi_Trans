#include <iostream>
#include <vector>
#include <cstdint>
#include <limits>
#include <cstring>

#include "./pack.hpp"

using namespace navi_trans;
class A {
    // add key version
    public:
        int32_t a;
        int32_t b;
        int32_t c;
        A() {}
        size_t do_pack_size() const {
            size_t payload_size = pack_size<uint32_t, INT_32>(generate_key<INT_32>(0));
            payload_size += pack_size<int32_t, INT_32>(a);
            payload_size += pack_size<uint32_t, INT_32>(generate_key<INT_32>(1));
            payload_size += pack_size<int32_t, INT_32>(b);
            payload_size += pack_size<uint32_t, INT_32>(generate_key<INT_32>(2));
            payload_size += pack_size<int32_t, INT_32>(c);
            return payload_size;
        }

        size_t do_pack(uint8_t* buf) const {
            size_t payload_size = pack_key<INT_32>(buf, 0);
            payload_size += pack<int32_t, INT_32>(buf + payload_size, a);
            payload_size += pack_key<INT_32>(buf + payload_size, 1);
            payload_size += pack<int32_t, INT_32>(buf + payload_size, b);
            payload_size += pack_key<INT_32>(buf + payload_size, 2);
            payload_size += pack<int32_t, INT_32>(buf + payload_size, c);
            return payload_size;
        }

        size_t do_unpack(uint8_t* buf, size_t& payload_size) {
            uint32_t field_number = 0;
            size_t offset = unpack_key<INT_32>(buf, field_number);
            offset += unpack<int32_t, INT_32>(buf + offset, a);
            offset += unpack_key<INT_32>(buf + offset, field_number);
            offset += unpack<int32_t, INT_32>(buf + offset, b);
            offset += unpack_key<INT_32>(buf + offset, field_number);
            offset += unpack<int32_t, INT_32>(buf + offset, c);
            return payload_size;
        }
};

class B {
    // does not add key version
    public:
        A a;
        int32_t b;
        int32_t c;
        B() {}
        size_t do_pack_size() const {
            size_t payload_size = pack_size<A, MESSAGE>(a) + pack_size<int32_t, INT_32>(b) + pack_size<int32_t, INT_32>(c);
            return payload_size;
        }

        size_t do_pack(uint8_t* buf) const {
            size_t payload_size = pack<A, MESSAGE>(buf, a);
            payload_size += pack<int32_t, INT_32>(buf + payload_size, b);
            payload_size += pack<int32_t, INT_32>(buf + payload_size, c);
            return payload_size;
        }

        size_t do_unpack(uint8_t* buf, size_t& payload_size) {
            size_t offset = unpack<A, MESSAGE>(buf, a);
            offset += unpack<int32_t, INT_32>(buf + offset, b);
            offset += unpack<int32_t, INT_32>(buf + offset, c);
            return payload_size;
        }
};

class C {
    public:
        Array<int32_t, INT_32> a;
        int32_t b;
        C() {}
        size_t do_pack_size() const {
            size_t payload_size = pack_size<Array<int32_t, INT_32>, ARRAY>(a) + pack_size<int32_t, INT_32>(b);
            return payload_size;
        }

        size_t do_pack(uint8_t* buf) const {
            size_t payload_size = pack<Array<int32_t, INT_32>, ARRAY>(buf, a);
            payload_size += pack<int32_t, INT_32>(buf + payload_size, b);
            return payload_size;
        }

        size_t do_unpack(uint8_t* buf, size_t& payload_size) {
            size_t offset = unpack<Array<int32_t, INT_32>, ARRAY>(buf, a);
            offset += unpack<int32_t, INT_32>(buf + offset, b);
            return payload_size;
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
    ps = pack_size<A, MESSAGE>(data4);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf4 = new uint8_t[ps];
    pack<A, MESSAGE>(buf4, data4);
    // unpack
    unpack<A, MESSAGE>(buf4, data4);
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
    ps = pack_size<B, MESSAGE>(data5);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf5 = new uint8_t[ps];
    pack<B, MESSAGE>(buf5, data5);
    // unpack
    unpack<B, MESSAGE>(buf5, data5);
    std::cout << data5.a.a << "\n";
    std::cout << data5.a.b << "\n";
    std::cout << data5.a.c << "\n";
    std::cout << data5.b << "\n";
    std::cout << data5.c << "\n";
    delete[] buf5;

    // message with array
    C data6;
    data6.a.data.push_back(5);
    data6.a.data.push_back(10);
    data6.a.data.push_back(15);
    data6.a.data.push_back(20);
    data6.b = 40;
    ps = pack_size<C, MESSAGE>(data6);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf6 = new uint8_t[ps];
    pack<C, MESSAGE>(buf6, data6);
    // unpack
    C data6_1;
    unpack<C, MESSAGE>(buf6, data6_1);
    std::cout << data6_1.a.data[0] << "\n";
    std::cout << data6_1.a.data[1] << "\n";
    std::cout << data6_1.a.data[2] << "\n";
    std::cout << data6_1.a.data[3] << "\n";
    std::cout << data6_1.b << "\n";
    delete[] buf6;

    // generate_key
    uint8_t* buf7 = new uint8_t[1];
    pack_key<MESSAGE>(buf7, 4);
    std::cout << std::hex << +buf7[0] << "\n";
    uint32_t field_number;
    unpack_key<MESSAGE>(buf7, field_number);
    std::cout << field_number<< "\n";
    delete[] buf7;
    return 0;
}