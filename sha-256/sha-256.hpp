#pragma once


#include <array>
#include <string>
#include <cstdint>


//For more information on magic numbers:
// https://en.wikipedia.org/wiki/SHA-2#Pseudocode
class SHA256 {
public:
    SHA256();
    SHA256(const char* hash);

    void update(const char* data, size_t length);
    void update(const uint8_t* data, size_t length);
    uint8_t* digest();

    static std::string to_string(const uint8_t* digest);

    bool operator==(const SHA256& rhs);

private:
    uint8_t  m_data[64]{};
    uint32_t m_block_length;
    uint64_t m_bit_length;
    uint32_t m_state[8]; //A, B, C, D, E, F, G, H
    std::array<uint8_t, 32> m_hash{};

    void pad();
    void transform();
    void revert();
};