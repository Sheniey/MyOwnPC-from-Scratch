
#include "cache.h"
#include "utils.h"
#include "imc.h"

Cache::Cache() : policies(*this), L1(*this), L2(*this) {  // inicialización de L1 y L2
  memset(&l2, 0, sizeof(l2));
  memset(&l1, 0, sizeof(l1));
  l1.instr.size = 24;
  l1.data.size = 8;
  l2.size = 64;
}

static void Cache::Policies::fifo(Cache& cache, CacheTier level, uint64_t addr, uint16_t newData, bool instr) {
  if (level == CacheTier::L2) {
    Utils::shiftLeftArray(cache.l2.payload, cache.l2.size);
    Utils::shiftLeftArray(cache.l2.index, cache.l2.size);
    Utils::shiftLeftArray(cache.l2.weight, cache.l2.size);
    cache.l2.payload[cache.l2.size - 1] = newData;
    cache.l2.index[cache.l2.size - 1] = addr;
    cache.l2.weight[cache.l2.size - 1] = 0;
  } else if (instr) {
    Utils::shiftLeftArray(cache.l1.instr.payload, cache.l1.instr.size);
    Utils::shiftLeftArray(cache.l1.instr.index, cache.l1.instr.size);
    Utils::shiftLeftArray(cache.l1.instr.weight, cache.l1.instr.size);
    cache.l1.instr.payload[cache.l1.instr.size - 1] = newData;
    cache.l1.instr.index[cache.l1.instr.size - 1] = addr;
    cache.l1.instr.weight[cache.l1.instr.size - 1] = 0;
  } else {
    Utils::shiftLeftArray(cache.l1.data.payload, cache.l1.data.size);
    Utils::shiftLeftArray(cache.l1.data.index, cache.l1.data.size);
    Utils::shiftLeftArray(cache.l1.data.weight, cache.l1.data.size);
    cache.l1.data.payload[cache.l1.data.size - 1] = newData;
    cache.l1.data.index[cache.l1.data.size - 1] = addr;
    cache.l1.data.weight[cache.l1.data.size - 1] = 0;
  }
}

// Minimum Weight
static void Cache::Policies::minWeight(Cache& cache, CacheTier level, uint64_t addr, uint16_t newData, bool instr) {
  uint8_t cell;
  if (level == CacheTier::L2) {
    cell = Utils::where(Utils::minArray(cache.l2.weight, cache.l2.size), cache.l2.weight, cache.l2.size);
    cache.l2.payload[cell] = newData;
    cache.l2.index[cell] = addr;
    cache.l2.weight[cell] = 0;
  } else if (instr) {
    cell = Utils::where(Utils::minArray(cache.l1.instr.weight, cache.l1.instr.size), cache.l1.instr.weight, cache.l1.instr.size);
    cache.l1.instr.payload[cell] = newData;
    cache.l1.instr.index[cell] = addr;
    cache.l1.instr.weight[cell] = 0;
  } else {
    cell = Utils::where(Utils::minArray(cache.l1.data.weight, cache.l1.data.size), cache.l1.data.weight, cache.l1.data.size);
    cache.l1.data.payload[cell] = newData;
    cache.l1.data.index[cell] = addr;
    cache.l1.data.weight[cell] = 0;
  }
}

// Least Recently Used (LRU)
static void Cache::Policies::lru(Cache& cache, CacheTier level, uint64_t addr, uint16_t newData, bool instr) {
  if (level == CacheTier::L2) {
    Utils::shiftLeftArray(cache.l2.payload, cache.l2.size);
    Utils::shiftLeftArray(cache.l2.index, cache.l2.size);
    Utils::shiftLeftArray(cache.l2.weight, cache.l2.size);
    cache.l2.payload[cache.l2.size - 1] = newData;
    cache.l2.index[cache.l2.size - 1] = addr;
    cache.l2.weight[cache.l2.size - 1] = 0;
  } else if (instr) {
    Utils::shiftLeftArray(cache.l1.instr.payload, cache.l1.instr.size);
    Utils::shiftLeftArray(cache.l1.instr.index, cache.l1.instr.size);
    Utils::shiftLeftArray(cache.l1.instr.weight, cache.l1.instr.size);
    cache.l1.instr.payload[cache.l1.instr.size - 1] = newData;
    cache.l1.instr.index[cache.l1.instr.size - 1] = addr;
    cache.l1.instr.weight[cache.l1.instr.size - 1] = 0;
  } else {
    Utils::shiftLeftArray(cache.l1.data.payload, cache.l1.data.size);
    Utils::shiftLeftArray(cache.l1.data.index, cache.l1.data.size);
    Utils::shiftLeftArray(cache.l1.data.weight, cache.l1.data.size);
    cache.l1.data.payload[cache.l1.data.size - 1] = newData;
    cache.l1.data.index[cache.l1.data.size - 1] = addr;
    cache.l1.data.weight[cache.l1.data.size - 1] = 0;
  }
}

// Clock / Second Chance
static void Cache::Policies::clock(Cache& cache, CacheTier level, uint64_t addr, uint16_t newData, bool instr) {
  static uint8_t handL1Instr = 0;
  static uint8_t handL1Data = 0;
  static uint8_t handL2 = 0;

  if (level == CacheTier::L2) {
    while (true) {
      if (cache.l2.weight[handL2] == 0) {
        cache.l2.payload[handL2] = newData;
        cache.l2.index[handL2] = addr;
        cache.l2.weight[handL2] = 1;
        handL2 = (handL2 + 1) % cache.l2.size;
        break;
      } else {
        cache.l2.weight[handL2] = 0;
        handL2 = (handL2 + 1) % cache.l2.size;
      }
    }
  } else if (instr) {
    while (true) {
      if (cache.l1.instr.weight[handL1Instr] == 0) {
        cache.l1.instr.payload[handL1Instr] = newData;
        cache.l1.instr.index[handL1Instr] = addr;
        cache.l1.instr.weight[handL1Instr] = 1;
        handL1Instr = (handL1Instr + 1) % cache.l1.instr.size;
        break;
      } else {
        cache.l1.instr.weight[handL1Instr] = 0;
        handL1Instr = (handL1Instr + 1) % cache.l1.instr.size;
      }
    }
  } else {
    while (true) {
      if (cache.l1.data.weight[handL1Data] == 0) {
        cache.l1.data.payload[handL1Data] = newData;
        cache.l1.data.index[handL1Data] = addr;
        cache.l1.data.weight[handL1Data] = 1;
        handL1Data = (handL1Data + 1) % cache.l1.data.size;
        break;
      } else {
        cache.l1.data.weight[handL1Data] = 0;
        handL1Data = (handL1Data + 1) % cache.l1.data.size;
      }
    }
  }
}

Cache::__L2Controller__& Cache::__L2Controller__::clear() {
  for (uint8_t i = 0; i < owner.l2.size; i++) {
    owner.l2.payload[i] = 0;
    owner.l2.index[i] = 0;
    owner.l2.weight[i] = 0;
  }
  return *this;
}

Cache::__L2Controller__& Cache::__L2Controller__::cacheMiss(
  uint64_t addr,
  void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool)
) {
  uint16_t newData = IMC::read(addr);
  if (Utils::into(true, owner.l2.modified, owner.l2.size)) {
    uint8_t cell = Utils::where(true, owner.l2.modified, owner.l2.size);
    IMC::write(owner.l2.index[cell], owner.l2.payload[cell]);
    owner.l2.payload[cell] = newData;
  } else policy(owner, CacheTier::L2, addr, newData, false);
  return *this;
}

uint16_t Cache::__L2Controller__::cacheHit(uint8_t cell) {
  owner.l2.weight[cell]++;
  return owner.l2.payload[cell];
}

uint16_t Cache::__L2Controller__::read(
  uint64_t addr,
  void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool)
) {
  if (!Utils::into(addr, owner.l2.index, owner.l2.size)) {
    return owner.L2.cacheMiss(addr, policy).read(addr, policy);
  }
  return owner.L2.cacheHit(Utils::where(addr, owner.l2.index, owner.l2.size));
}

/** [!] IMPORTANT:
* WriteModes:       [ Write-Through, Write-Back ]
* WriteMode Used:   | Write-Back |
* Description: 
Cache& Cache::__L2Controller__::write(
  uint64_t addr,
  uint16_t data,
  void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool)
) {
  policy(owner, CacheTier::L2, addr, data, false);
  owner.l2.modified[Utils::where(data, owner.l2.payload, owner.l2.size)] = true;
  return *this;
}

Cache::__L1Controller__& Cache::__L1Controller__::clear() {
  for (uint8_t i = 0; i < owner.l1.instr.size; i++) {
    owner.l1.instr.payload[i] = 0;
    owner.l1.instr.index[i] = 0;
    owner.l1.instr.weight[i] = 0;
  }

  for (uint8_t i = 0; i < owner.l1.data.size; i++) {
    owner.l1.data.payload[i] = 0;
    owner.l1.data.index[i] = 0;
    owner.l1.data.weight[i] = 0;
  }
  return *this;
}

Cache::__L1Controller__& Cache::__L1Controller__::cacheMiss(
  uint64_t addr,
  void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool),
  bool instr
) {
  uint16_t newData = owner.L2.read(addr);
  if (instr == true && Utils::into(true, owner.l1.data.modified, owner.l1. data.size)) {
    uint8_t cell = Utils::where(true, owner.l1.data.modified, owner.l1.data.size);
    owner.L2.write(owner.l1.data.index[cell], owner.l1.data.payload[cell]);
    owner.l1.data.payload[cell] = newData;
  } else policy(owner, CacheTier::L1, addr, newData, instr);
  return *this;
}

uint16_t Cache::__L1Controller__::cacheHit(uint8_t cell, bool instr) {
  if (instr) {
    owner.l1.instr.weight[cell]++;
    return owner.l1.instr.payload[cell];
  } else {
    owner.l1.data.weight[cell]++;
    return owner.l1.data.payload[cell];
  }
}

uint16_t Cache::__L1Controller__::readInstr(
  uint64_t addr,
  void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool)
) {
  if (!Utils::into(addr, owner.l1.instr.index, owner.l1.instr.size)) {
    return owner.L1.cacheMiss(addr, policy, true).readInstr(addr, policy);
  }
  return owner.L1.cacheHit(Utils::where(addr, owner.l1.instr.index, owner.l1.instr.size), true);
}

uint16_t Cache::__L1Controller__::readData(
  uint64_t addr,
  void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool)
) {
  if (!Utils::into(addr, owner.l1.data.index, owner.l1.data.size)) {
    return owner.L1.cacheMiss(addr, policy, false).readData(addr, policy);
  }
  return owner.L1.cacheHit(Utils::where(addr, owner.l1.data.index, owner.l1.data.size));
}

Cache& Cache::__L1Controller__::write(
  uint64_t addr,
  uint16_t data,
  void (*policy)(Cache&, CacheTier, uint64_t, uint16_t, bool)
) {
  policy(owner, CacheTier::L1, addr, data, false);
  owner.l1.data.modified[Utils::where(data, owner.l1.data.payload, owner.l1.data.size)] = true;
  return *this;
}
