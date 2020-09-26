#include <iostream>
#include <pack.hpp>


using namespace navi_trans;

class B {
    // add key version
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

template<> struct navi_trans::MemberListTraits<B> { 
    using m_list = MemberList<Member<decltype(&B::a), &B::a>, Member<decltype(&B::b), &B::b>, Member<decltype(&B::c), &B::c>>; }; 

template<> struct navi_trans::MemberListTraits<A> { 
    using m_list = MemberList<Member<decltype(&A::a), &A::a>, Member<decltype(&A::b), &A::b>, Member<decltype(&A::c), &A::c>, Member<decltype(&A::d), &A::d>>; }; 


int main() {
    int32_t data = -300;
    uint32_t ps = pack_size(data);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf = new uint8_t[ps];
    pack(buf, data);
    memset(&data, 0, sizeof(data));
    unpack(buf, data);
    std::cout << data << "\n";
    delete[] buf;

    int16_t data1 = -320;
    ps = pack_size(data1);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf1 = new uint8_t[ps];
    pack(buf1, data1);
    memset(&data1, 0, sizeof(data1));
    unpack(buf1, data1);
    std::cout << data1 << "\n";
    delete[] buf1;

    float data2 = 3.2;
    ps = pack_size(data2);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf2 = new uint8_t[ps];
    pack(buf2, data2);
    memset(&data2, 0, sizeof(data2));
    unpack(buf2, data2);
    std::cout << data2 << "\n";
    delete[] buf2;

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