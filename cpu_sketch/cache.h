
#pragma once
#include <Arduino.h>

enum class CacheTier { L1, L2 };

class Cache {
private:
    friend class __L2Controller__;
    friend class __L1Controller__;

    // Cache Level 2 : 128 Bytes
    struct {
        uint16_t payload[64];
        bool modified[64];
        uint64_t index[64];
        uint8_t weight[64];
        uint8_t size;
    } l2;

    // Cache Level 1 : 64 Bytes
    union {
        struct { // Payload : 48 Bytes
            uint16_t payload[24];
            uint64_t index[24];
            uint8_t weight[24];
            uint8_t size;
        } instr;

        struct { // Payload : 16 Bytes
            uint16_t payload[8];
            bool modified[8];
            uint64_t index[8];
            uint8_t weight[8];
            uint8_t size;
        } data;
    } l1;

public:
    Cache();

    class Policies {
    private:
        Cache& owner;

    public:
        Policies(Cache& c) : owner(c) {}

        // First-In First-Out 
        static void fifo(Cache& cache, CacheTier level, uint64_t addr, uint16_t newData, bool instr = false);
        // Minimum Weight
        static void minWeight(Cache& cache, CacheTier level, uint64_t addr, uint16_t newData, bool instr = false);
        // Least Recently Used
        static void lru(Cache& cache, CacheTier level, uint64_t addr, uint16_t newData, bool instr = false);
        // Clock
        static void clock(Cache& cache, CacheTier level, uint64_t addr, uint16_t newData, bool instr = false);
    };

    class __L2Controller__ {
    private:
        Cache& owner;

    public:
        __L2Controller__(Cache& c) : owner(c) {}
        __L2Controller__& clear();
        
        __L2Controller__& cacheMiss(uint64_t addr, void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool));
        uint16_t cacheHit(uint8_t cell); 
    
        uint16_t read(uint64_t addr, void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool) = Policies::clock);
        uint16_t write(uint64_t addr, uint16_t data, void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool) = Policies::clock);
    };

    class __L1Controller__ {
    private:
        Cache& owner;

    public:
        __L1Controller__(Cache& c) : owner(c) {}
        __L1Controller__& clear();

        __L1Controller__& cacheMiss(uint64_t addr, void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool), bool instr = false);
        uint16_t cacheHit(uint8_t cell, bool instr = false);

        uint16_t readInstr(uint64_t addr, void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool) = Policies::lru);
        uint16_t readData(uint64_t addr, void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool) = Policies::lru);
        void write(uint64_t addr, uint16_t data, void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool) = Policies::lru);
    };

    Policies policies;
    __L2Controller__ L2;
    __L1Controller__ L1;
};
