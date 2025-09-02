
#include "core.h"

void IRAM_ATTR intr() {
    
}

void IRAM_ATTR nmi() {
    
}

void setup() {
    beginIO();
    Core cpu = Core();
}

void Dumper() {
    cpu.dump();
}

void loop() {
    
}
