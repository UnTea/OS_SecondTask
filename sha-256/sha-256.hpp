#pragma once


#include <array>
#include <string>
#include <vector>


//For more information on magic numbers:
// https://en.wikipedia.org/wiki/SHA-2#Pseudocode
class SHA256 {
public:
    SHA256();

    void update(const uint8_t* data, size_t length);
    void update(const std::string& data);
    uint8_t* digest();

    static std::string to_string(const uint8_t* digest);

private:
    uint8_t  m_data[64]{};
    uint32_t m_block_length;
    uint64_t m_bit_length;
    uint32_t m_state[8]; //A, B, C, D, E, F, G, H

    void pad();
    void transform();
    void revert(uint8_t* hash);
};