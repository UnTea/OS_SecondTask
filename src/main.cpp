#include <sha-256.hpp>


#include <chrono>
#include <iostream>


int main() {
    std::string input {"zyzzx"};

    SHA256 sha;
    sha.update(input);
    uint8_t* output = sha.digest();

    std::cout << SHA256::to_string(output) << std::endl;

    delete[] output;
}
