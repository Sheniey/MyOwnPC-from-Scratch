
#pragma once
#include <Arduino.h>
#include "core.h"

// Ignora este comentario anterior, Aqui me ubiese gustado poner comentarios con los opcodes pero si hay un cambio en la ISA va a ser un lio.

inline std::vector<InstrOfISA> x86 = {
  { &Core::_nop, OperandFormat::NOTHING, CPL::User },
  { &Core::_hlt, OperandFormat::NOTHING, CPL::Kernel },
  
  { &Core::_add, OperandFormat::REG_IMM, CPL::User },
  { &Core::_add, OperandFormat::REG_REG, CPL::User },
  { &Core::_add, OperandFormat::REG_MEM, CPL::User },
  { &Core::_add, OperandFormat::MEM_IMM, CPL::User },
  { &Core::_add, OperandFormat::MEM_REG, CPL::User }
};

inline std::vector<InstrOfISA> err = {};

struct {
	std::vector<std::vector<InstrOfISA>> isa[] = {
    &x86,
    &err
  };

  uint16_t size();
  InstrOfISA get(uint16_t opcode);
} ISA;
