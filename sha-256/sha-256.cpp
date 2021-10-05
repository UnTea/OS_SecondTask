#include "sha-256.hpp"


#include <sstream>
#include <iomanip>


static constexpr std::array<uint32_t, 64> SHA256_KEYS {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

SHA256::SHA256()
: m_block_length(0)
, m_bit_length(0)
, m_state {
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19
}
{}

void SHA256::update(const uint8_t* data, size_t length) {
    for (size_t i = 0 ; i < length ; i++) {
        m_data[m_block_length++] = data[i];

        if (m_block_length == 64) {
            transform();

            m_bit_length += 512;
            m_block_length = 0;
        }
    }
}

void SHA256::update(const char* data, size_t length) {
    update(reinterpret_cast<const uint8_t*> (data), length);
}

uint8_t* SHA256::digest() {
    pad();
    revert();

    return m_hash.data();
}

uint32_t rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

uint32_t choose(uint32_t e, uint32_t f, uint32_t g) {
    return (e & f) ^ (~e & g);
}

uint32_t majority(uint32_t a, uint32_t b, uint32_t c) {
    return (a & (b | c)) | (b & c);
}

uint32_t sig0(uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

uint32_t sig1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

void SHA256::transform() {
    uint32_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
    uint32_t state[8];

    for (uint8_t i = 0, j = 0; i < 16; i++, j += 4) { // Split data in 32 bit blocks for the 16 first words
        m[i] =    (m_data[j    ] << 24)
                | (m_data[j + 1] << 16)
                | (m_data[j + 2] << 8 )
                | (m_data[j + 3]      );
    }

    for (uint8_t k = 16 ; k < 64; k++) { // Remaining 48 blocks
        m[k] = sig1(m[k - 2]) + m[k - 7] + sig0(m[k - 15]) + m[k - 16];
    }

    for(uint8_t i = 0 ; i < 8 ; i++) {
        state[i] = m_state[i];
    }

    for (uint8_t i = 0; i < 64; i++) {
        maj  = majority(state[0], state[1], state[2]);
        xorA = rotr(state[0], 2) ^ rotr(state[0], 13) ^ rotr(state[0], 22);
        ch   = choose(state[4], state[5], state[6]);
        xorE = rotr(state[4], 6) ^ rotr(state[4], 11) ^ rotr(state[4], 25);

        sum  = m[i] + SHA256_KEYS[i] + state[7] + ch + xorE;
        newA = xorA + maj + sum;
        newE = state[3] + sum;

        state[7] = state[6];
        state[6] = state[5];
        state[5] = state[4];
        state[4] = newE;
        state[3] = state[2];
        state[2] = state[1];
        state[1] = state[0];
        state[0] = newA;
    }

    for(uint8_t i = 0 ; i < 8 ; i++) {
        m_state[i] += state[i];
    }
}

void SHA256::pad() {
    uint64_t i = m_block_length;
    uint8_t end = m_block_length < 56 ? 56 : 64;

    m_data[i++] = 0x80; // Append a bit 1

    while (i < end) {
        m_data[i++] = 0x00; // Pad with zeros
    }

    if(m_block_length >= 56) {
        transform();
        memset(m_data, 0, 56);
    }

    // Append to the pad the total message's length in bits and transform.
    m_bit_length += m_block_length * 8;
    m_data[63] = m_bit_length;
    m_data[62] = m_bit_length >> 8;
    m_data[61] = m_bit_length >> 16;
    m_data[60] = m_bit_length >> 24;
    m_data[59] = m_bit_length >> 32;
    m_data[58] = m_bit_length >> 40;
    m_data[57] = m_bit_length >> 48;
    m_data[56] = m_bit_length >> 56;

    transform();
}

void SHA256::revert() {
    // SHA uses big endian byte ordering
    // Revert all bytes
    for (uint8_t i = 0 ; i < 4 ; i++) {
        for(uint8_t j = 0 ; j < 8 ; j++) {
            m_hash[i + (j * 4)] = (m_state[j] >> (24 - i * 8)) & 0x000000ff;
        }
    }
}

std::string SHA256::to_string(const uint8_t* digest) {
    std::stringstream s;
    s << std::setfill('0') << std::hex;

    for (uint8_t i = 0 ; i < 32 ; i++) {
        s << std::setw(2) << (size_t) digest[i];
    }

    return s.str();
}

int char_to_integer(char input) {
    if(input >= '0' && input <= '9') {
        return input - '0';
    }

    if(input >= 'A' && input <= 'F') {
        return input - 'A' + 10;
    }

    if(input >= 'a' && input <= 'f') {
        return input - 'a' + 10;
    }

    throw std::invalid_argument("Invalid input string");
}

void hex_to_binary(const char* hash, uint8_t* target) {
    while(*hash && hash[1]) {
        *(target++) = char_to_integer(*hash) * 16 + char_to_integer(hash[1]);
        hash += 2;
    }
}

SHA256::SHA256(const char* hash) {
    hex_to_binary(hash, m_hash.data());
}

bool SHA256::operator==(const SHA256& rhs) {
    return m_hash == rhs.m_hash;
}
