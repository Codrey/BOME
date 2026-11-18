#include <iostream>
#include <concepts>

template<typename T>
requires std::integral<T>
T add(T a, T b) {
    return a + b;
}

int main() {
    std::cout << "C++20 Environment Ready! 5 + 10 = " << add(5, 10) << std::endl;
    return 0;
}
