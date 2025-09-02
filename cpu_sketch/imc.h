
#pragma once
#include <Arduino.h>

namespace IMC {
    // nota para el futuro...
    //   -  (uint26_t)uint32_t
    // a pesar de ser una addr de 32 bits solo se tomaran en cuenta los primeros 26 bits (64 MiB)
    // si hay un page fault entonces se utiliza la addr: (uint26_t)(-1)
    uint16_t read(uint64_t addr);
    void write(uint64_t addr, uint16_t data);
};

namespace MMU {

};
