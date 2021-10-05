#include <sha-256.hpp>


#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>


int main() {
    const std::array<std::string, 26> dictionary {
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
        "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
    };

    std::vector<SHA256> hashes {
        "1115dd800feaacefdf481f1f9070374a2a81e27880f187396db67958b207cbad",
        "3a7bd3e2360a3d29eea436fcfb7e44c735d117c42d1c1835420b6b9942dd4f1b",
        "74e1bb62f8dabb8125a58852b63bdf6eaef667cb56ac7f7cdba6d7305c50a22f"
    };

    auto start = std::chrono::high_resolution_clock::now();
    uint8_t* output;

    for (const auto &i1: dictionary) {
        for (const auto &i2: dictionary) {
            for (const auto &i3: dictionary) {
                for (const auto &i4: dictionary) {
                    for (const auto &i5: dictionary) {
                        SHA256 sha;
                        std::string string {i1};
                        sha.update(string.append(i2).append(i3).append(i4).append(i5));
                        output = sha.digest();
                        auto iterator = std::find(hashes.begin(), hashes.end(), sha);

                        if (iterator != hashes.end()) {
                            std::cout << "Pass for hash " << SHA256::to_string(output) << " is " << string << std::endl;
                        }
                    }
                }
            }
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;
}

