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

enum fruit {
    apple = 1,
    peach,
    pear,
};

class A {
    public:
        int16_t a;
        double b;
        std::vector<int32_t> c;
        B d;
        enum fruit e;
        std::string f;
        double g[3];
        A() {}
};


template<> struct navi_trans::MemberListTraits<B> { 
    using m_list = MemberList<
        Member<decltype(&B::a), &B::a>, 
        Member<decltype(&B::b), &B::b>, 
        Member<decltype(&B::c), &B::c>>; }; 

template<> struct navi_trans::MemberListTraits<A> { 
    using m_list = MemberList<
        Member<decltype(&A::a), &A::a>, 
        Member<decltype(&A::b), &A::b>, 
        Member<decltype(&A::c), &A::c>, 
        Member<decltype(&A::d), &A::d>,
        Member<decltype(&A::e), &A::e>,
        Member<decltype(&A::f), &A::f>,
        Member<decltype(&A::g), &A::g>>; }; 

int main() {
    B b;
    b.a = 1;
    b.b = 2;
    b.c = 3;
    
    A a;
    a.a = 4;
    a.b = 5;
    a.c.resize(3);
    a.c[0] = 6;
    a.c[1] = 7;
    a.c[2] = 8;
    a.d = b;
    a.e = peach;
    a.f = "hello world";
    a.g[0] = 9;
    a.g[1] = 10;
    a.g[2] = 11;
    uint32_t ps = pack_size(a);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf = new uint8_t[ps];
    pack(buf, a);
    A new_a;

    // unpack
    unpack(buf, new_a);
    std::cout << "a.a:      " << a.a << "    " << new_a.a << "\n";
    std::cout << "a.b:      " << a.b << "    " << new_a.b << "\n";
    std::cout << "a.c[0]:       " << a.c[0] << "    " << new_a.c[0] << "\n";
    std::cout << "a.c[1]:       " << a.c[1] << "    " << new_a.c[1] << "\n";
    std::cout << "a.c[2]:       " << a.c[2] << "    " << new_a.c[2] << "\n";
    std::cout << "a.d.a:        " << a.d.a << "    " << new_a.d.a << "\n";
    std::cout << "a.d.b:        " << a.d.b << "    " << new_a.d.b << "\n";
    std::cout << "a.d.c:        " << a.d.c << "    " << new_a.d.c << "\n";
    std::cout << "a.e:      " << a.e << "    " << new_a.e << "\n";
    std::cout << "a.f:      " << a.f << "    " << new_a.f << "\n";
    std::cout << "a.g[0]:       " << a.g[0] << "    " << new_a.g[0] << "\n";
    std::cout << "a.g[1]:       " << a.g[1] << "    " << new_a.g[1] << "\n";
    std::cout << "a.g[2]:       " << a.g[2] << "    " << new_a.g[2] << "\n";
    delete[] buf;

    return 0;
}