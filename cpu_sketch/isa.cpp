
#include "isa.h"

/*
* OPERAND ORDER:
*   nothing*
*   reg, imm
*   reg, reg
*   reg, mem
*   mem, imm
*   mem, reg
*   imm
*   reg
*   mem
*/
std::array<uint8_t, 5> acceptedClassicFormats = {
  OperandFormat::REG_IMM,
  OperandFormat::REG_REG,
  OperandFormat::REG_MEM,
  OperandFormat::MEM_IMM,
  OperandFormat::MEM_REG 
};

void Core::_nop(InstrPackage package, OperandFormat format, CPL privilege) return;
void Core::_hlt(InstrPackage package, OperandFormat format, CPL privilege) {
  if (InstrDevelopmentTools.checkNecessaryCPL()) { this->halted = true; }
  else { stderr::RequiredPermits(); }
}
void Core::_add(InstrPackage package, OperandFormat format, CPL privilege) {
  uint16_t _words[4];

  std::vector<void*> f = {
    auto _add_reg_imm = [this](package) -> void {
      DataType datatype = package.instr.datatype;

      registers.write(
        package.modrm.rm,
        InstrDevelopmentTools.formatArgument(
          datatype,
          registers.read(
            package.modrm.rm,
            InstrDevelopmentTools.datatype2buffer[datatype][0]
          ),
          false
        ) +
        InstrDevelopmentTools.formatArgument(
          datatype,
          Utils::i16toi64(package.arg0, package.arg1, package.arg2, package.arg3),
          true
        ),
        InstrDevelopmentTools.datatype2buffer[datatype][0]
      );
    };
    
    auto _add_reg_reg = [this](package) -> void {
      DataType datatype = package.instr.datatype;

      registers.write(
        package.modrm.rm,
        InstrDevelopmentTools.formatArgument(
          datatype,
          registers.read(
            package.modrm.rm,
            InstrDevelopmentTools.datatype2buffer[datatype][0]
          ),
          false
        ) + 
        InstrDevelopmentTools.formatArgument(
          datatype,
          registers.read(
            package.modrm.reg,
            InstrDevelopmentTools.datatype2buffer[datatype][1]
          ),
          true
        ),
        InstrDevelopmentTools.datatype2buffer[datatype][0]
      );
    };

    auto _add_reg_mem = [this](package) -> void {
      uint64_t memaddr = (
        0x0 <= package.modrm.mode <= 0x2 ? Utils::i16toi32(package.arg0, package.arg1) :
        0x3 <= package.modrm.mode <= 0x5 ? registers.read(package.modrm.reg, RegisterBuffer::Double) :
        package.modrm.mode == 0x6 ? 
      ) + package.disp;
      DataType datatype = package.instr.datatype;

      registers.write(
        package.modrm.rm,
        InstrDevelopmentTools.formatArgument(
          datatype,
          registers.read(
            package.modrm.rm,
            InstrDevelopmentTools.datatype2buffer[datatype][0]
          ),
          false
        ) +
        InstrDevelopmentTools.formatArgument(
          datatype,
          cache.L1.readData(memaddr),
          true
        ),
        InstrDevelopmentTools.datatype2buffer[datatype][0]
      );
    };

    auto _add_mem_imm = [this](package) -> void {
      uint64_t memaddr = Utils::i16toi64(
        package.arg0,
        package.arg1,
        package.arg2,
        package.arg3
      ) + package.disp;
      DataType datatype = package.instr.datatype;

      auto res = InstrDevelopmentTools.formatArgument(datatype, cache.L1.readData(memaddr), false) +
                 InstrDevelopmentTools.formatArgument(datatype, Utils::i16toi64(package.arg4, package.arg5, package.arg6, package.arg7), true);
      _words[0] = (uint16_t)(res & 0xFFFF);
      _words[1] = (uint16_t)((res >> 16) & 0xFFFF);
      _words[2] = (uint16_t)((res >> 32) & 0xFFFF);
      _words[3] = (uint16_t)((res >> 48) & 0xFFFF);

      // InstrDevelopmentTools.datatype2buffer devuelve, a partir de un DataType, el Buffer para los Registros.
      // Estos también incluyen Byte (Low, High) por ello usamos -1 para indicar que queremos múltiplos de Word (16b).
      // De esta forma descartamos ByteHigh, y como ByteLow esta en el índice 0, no nos afecta en este caso.
      for (uint8_t word = 0; word < (InstrDevelopmentTools.datatype2buffer[datatype][0] - 1); word++) {
        cache.L1.write(memaddr + word, _words[word]);
      }
    };

    auto _add_mem_reg = [this](package) -> void {
      uint64_t memaddr = Utils::i16toi64(
        package.arg0,
        package.arg1,
        package.arg2,
        package.arg3
      ) + package.disp;
      DataType datatype = package.instr.datatype;
      
      auto res = InstrDevelopmentTools.formatArgument(datatype, cache.L1.readData(memaddr), false) +
                 InstrDevelopmentTools.formatArgument(datatype, registers.read(package.modrm.reg, InstrDevelopmentTools.datatype2buffer[datatype][1]), true);
      _words[0] = (uint16_t)(res & 0xFFFF);
      _words[1] = (uint16_t)((res >> 16) & 0xFFFF);
      _words[2] = (uint16_t)((res >> 32) & 0xFFFF);
      _words[3] = (uint16_t)((res >> 48) & 0xFFFF);

      // InstrDevelopmentTools.datatype2buffer devuelve, a partir de un DataType, el Buffer para los Registros.
      // Estos también incluyen Byte (Low, High) por ello usamos -1 para indicar que queremos múltiplos de Word (16b).
      // De esta forma descartamos ByteHigh, y como ByteLow esta en el índice 0, no nos afecta en este caso.
      for (uint8_t word = 0; word < (InstrDevelopmentTools.datatype2buffer[datatype][0] - 1); word++) {
        cache.L1.write(memaddr + word, _words[word]);
      }
    };
  };

  if (Utils::into(format, acceptedClassicFormats, 5)) { f.at(format)(package); }
  else { stderr::InstructionFormatUnrecognized() }
}

uint16_t ISA::size() {
  uint16_t summation = 0;
  for (uint8_t set = 0; set <= ISA.isa.size(); set++) {
    summation += ISA.isa[set].size();
  }
}

InstrOfISA ISA::get(uint16_t opcode) {
  uint16_t instructions = this->size();
  uint16_t summation = 0;

  for (uint8_t set = 0; set <= ISA.isa.size(); set++) {
    uint16_t currentSize = ISA.isa[set].size();
    if (currentSize <= opcode <= (currentSize + summation)) {
      return ISA.isa.at(opcode - summation);
    } else {
      summation += currentSize;
    }
  }
}
