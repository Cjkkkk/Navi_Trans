#include <iostream>
#include "./message_A.hpp"

int main() {
    B data;
    data.a = -20;
    data.b = 15;
    size_t ps = pack_size<MESSAGE>(data);
    std::cout << "pack_size: " << ps << "\n";
    uint8_t* buf = new uint8_t[ps];
    pack<MESSAGE>(buf, data);
    // unpack
    unpack<MESSAGE>(buf, data);
    std::cout << data.a << "\n";
    std::cout << data.b << "\n";
    delete[] buf;
}