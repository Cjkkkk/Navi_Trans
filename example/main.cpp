#include <iostream>
#include <pack.hpp>
#include <builtin.hpp>

using namespace navi_trans;

class B {
    public:
        int32_t a;
        double b;
        int32_t c;
        B() {}
};

class A {
    public:
        int32_t a;
        double b;
        int32_t c;
        B d;
        A() {}
};


class C {
    public:
        int32_t a;
        std::vector<int32_t> b;
        C() {}
};

enum fruit {
    apple = 1,
    peach,
    pear,
};

template<> struct navi_trans::MemberListTraits<B> { 
    using m_list = MemberList<Member<decltype(&B::a), &B::a>, Member<decltype(&B::b), &B::b>, Member<decltype(&B::c), &B::c>>; }; 

template<> struct navi_trans::MemberListTraits<A> { 
    using m_list = MemberList<Member<decltype(&A::a), &A::a>, Member<decltype(&A::b), &A::b>, Member<decltype(&A::c), &A::c>, Member<decltype(&A::d), &A::d>>; }; 

template<> struct navi_trans::MemberListTraits<C> { 
    using m_list = MemberList<Member<decltype(&C::a), &C::a>, Member<decltype(&C::b), &C::b>>; }; 

int main() {
    // bit32
    int32_t data = -300;
    uint32_t ps = pack_size(data);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf = new uint8_t[ps];
    pack(buf, data);
    memset(&data, 0, sizeof(data));
    unpack(buf, data);
    std::cout << data << "\n";
    delete[] buf;

    // varint
    int16_t data1 = -320;
    ps = pack_size(data1);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf1 = new uint8_t[ps];
    pack(buf1, data1);
    memset(&data1, 0, sizeof(data1));
    unpack(buf1, data1);
    std::cout << data1 << "\n";
    delete[] buf1;

    // enum
    enum fruit data2 = pear;
    ps = pack_size(data2);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf2 = new uint8_t[ps];
    pack(buf2, data2);
    memset(&data2, 0, sizeof(data2));
    unpack(buf2, data2);
    std::cout << data2 << "\n";
    delete[] buf2;

    // bit64
    double data3 = 3.4;
    ps = pack_size(data3);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf3 = new uint8_t[ps];
    pack(buf3, data3);
    memset(&data3, 0, sizeof(data3));
    unpack(buf3, data3);
    std::cout << data3 << "\n";
    delete[] buf3;

    // message
    B data4;
    data4.a = 10;
    data4.b = 15.5;
    data4.c = 20;
    ps = pack_size(data4);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf4 = new uint8_t[ps];
    pack(buf4, data4);
    memset(&data4, 0, sizeof(data4));
    // unpack
    unpack(buf4, data4);
    std::cout << data4.a << "\n";
    std::cout << data4.b << "\n";
    std::cout << data4.c << "\n";
    delete[] buf4;


    // nest message
    A data5;
    data5.a = 1;
    data5.b = 2;
    data5.c = 3;
    data5.d = data4;
    ps = pack_size(data5);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf5 = new uint8_t[ps];
    pack(buf5, data5);
    memset(&data5, 0, sizeof(data5));
    // unpack
    unpack(buf5, data5);
    std::cout << data5.a << "\n";
    std::cout << data5.b << "\n";
    std::cout << data5.c << "\n";
    std::cout << data5.d.a << "\n";
    std::cout << data5.d.b << "\n";
    std::cout << data5.d.c << "\n";
    delete[] buf5;


    // vector
    std::vector<uint32_t> data6(3);
    data6[0] = 1;
    data6[1] = 2;
    data6[2] = 3;
    ps = pack_size(data6);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf6 = new uint8_t[ps];
    pack(buf6, data6);
    data6[1] = 0;
    data6[2] = 0;
    data6[3] = 0;
    // unpack
    unpack(buf6, data6);
    std::cout << data6[0] << "\n";
    std::cout << data6[1] << "\n";
    std::cout << data6[2] << "\n";


    // nest message
    C data8;
    data8.a = 1;
    data8.b.resize(3);
    data8.b[0] = 2;
    data8.b[1] = 3;
    data8.b[2] = 4;
    ps = pack_size(data8);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf8 = new uint8_t[ps];
    pack(buf8, data8);
    // unpack
    unpack(buf8, data8);
    std::cout << data8.a << "\n";
    std::cout << data8.b[0] << "\n";
    std::cout << data8.b[1] << "\n";
    std::cout << data8.b[2] << "\n";
    delete[] buf8;


    // string
    std::string data9 = "123456";
    ps = pack_size(data9);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf9 = new uint8_t[ps];
    pack(buf9, data9);
    // unpack
    unpack(buf9, data9);
    std::cout << data9 << "\n";
    delete[] buf9;

    // vector
    uint32_t data10[3];
    data10[0] = 1;
    data10[1] = 2;
    data10[2] = 3;
    ps = pack_size(data10);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf10 = new uint8_t[ps];
    pack(buf10, data10);
    data10[0] = 0;
    data10[1] = 0;
    data10[2] = 0;
    // unpack
    unpack(buf10, data10);
    std::cout << data10[0] << "\n";
    std::cout << data10[1] << "\n";
    std::cout << data10[2] << "\n";
    delete[] buf10;

    // generate_key
    uint8_t* buf7 = new uint8_t[1];
    uint32_t field_number = 4;
    pack_key<uint32_t>(buf7, field_number);
    std::cout << std::hex << +buf7[0] << "\n";
    unpack_key<uint32_t>(buf7, field_number);
    std::cout << field_number<< "\n";
    delete[] buf7;

    return 0;
}