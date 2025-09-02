
#include "utils.h"
#include "io.h"

namespace IMC {
    uint16_t read(uint64_t addr) {
        uint8_t lo = stdin::memory(addr); uint8_t hi = stdin::memory(addr);
        return Utils::i8toi16(lo, hi);
    }

    void write(uint64_t addr, uint16_t data) {
        uint8_t hi, lo;
        hi, lo = Utils::i16toi8(data);
        stdout::memory(hi); stdout::memory(lo);
    }
};

namespace MMU {

};