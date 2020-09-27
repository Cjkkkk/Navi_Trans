#include <iostream>

template <typename T, size_t N>
inline uint32_t get_size(T (&data)[N]) {
    std::cout << sizeof(T) << "\n";
    std::cout << sizeof(T[0]) << "\n";
    return 1;
}

int main() {
    int a[3];
    std::cout << get_size(a);
}