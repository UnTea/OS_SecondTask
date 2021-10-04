#include "sha-256.hpp"


#include <sstream>
#include <iomanip>


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

void SHA256::update(const std::string& data) {
    update(reinterpret_cast<const uint8_t*> (data.c_str()), data.size());
}

uint8_t* SHA256::digest() {
    uint8_t* hash = new uint8_t[32];

    pad();
    revert(hash);

    return hash;
}

uint32_t SHA256::rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

uint32_t SHA256::choose(uint32_t e, uint32_t f, uint32_t g) {
    return (e & f) ^ (~e & g);
}

uint32_t SHA256::majority(uint32_t a, uint32_t b, uint32_t c) {
    return (a & (b | c)) | (b & c);
}

uint32_t SHA256::sig0(uint32_t x) {
    return SHA256::rotr(x, 7) ^ SHA256::rotr(x, 18) ^ (x >> 3);
}

uint32_t SHA256::sig1(uint32_t x) {
    return SHA256::rotr(x, 17) ^ SHA256::rotr(x, 19) ^ (x >> 10);
}

void SHA256::transform() {
    uint32_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
    uint32_t state[8];

    for (uint8_t i = 0, j = 0; i < 16; i++, j += 4) { // Split data in 32 bit blocks for the 16 first words
        m[i] =    (m_data[j]     << 24)
                | (m_data[j + 1] << 16)
                | (m_data[j + 2] << 8 )
                | (m_data[j + 3]      );
    }

    for (uint8_t k = 16 ; k < 64; k++) { // Remaining 48 blocks
        m[k] = SHA256::sig1(m[k - 2]) + m[k - 7] + SHA256::sig0(m[k - 15]) + m[k - 16];
    }

    for(uint8_t i = 0 ; i < 8 ; i++) {
        state[i] = m_state[i];
    }

    for (uint8_t i = 0; i < 64; i++) {
        maj   = SHA256::majority(state[0], state[1], state[2]);
        xorA  = SHA256::rotr(state[0], 2) ^ SHA256::rotr(state[0], 13) ^ SHA256::rotr(state[0], 22);

        ch = choose(state[4], state[5], state[6]);

        xorE  = SHA256::rotr(state[4], 6) ^ SHA256::rotr(state[4], 11) ^ SHA256::rotr(state[4], 25);

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

void SHA256::revert(uint8_t* hash) {
    // SHA uses big endian byte ordering
    // Revert all bytes
    for (uint8_t i = 0 ; i < 4 ; i++) {
        for(uint8_t j = 0 ; j < 8 ; j++) {
            hash[i + (j * 4)] = (m_state[j] >> (24 - i * 8)) & 0x000000ff;
        }
    }
}

std::string SHA256::toString(const uint8_t* digest) {
    std::stringstream s;
    s << std::setfill('0') << std::hex;

    for(uint8_t i = 0 ; i < 32 ; i++) {
        s << std::setw(2) << (unsigned int) digest[i];
    }

    return s.str();
}