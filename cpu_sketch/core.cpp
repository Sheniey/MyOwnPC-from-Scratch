#include <type_traits>
#include <stdexcept>

#include "core.h"
#include "io.h"

Core::Core() {
  uint16_t lenISA = ISA.size();
}

// Base + ( Index * Scale ) + disp16
uint32_t Core::InstrDevelopmentTools::resolveSIB(InstrPackage::sib sib, InstrPackage::modrm modrm) {
  uint8_t scales = {1, 2, 4, 8};
  if (modrm.mode == 0x6) {
    return registers.read(sib.base, RegisterBuffer::Double) + (scales[sib.scale] * sib.index)
  } else if (modrm.mode == 0x7) {
    return registers.read(sib.base, RegisterBuffer::Double) + (scales[sib.scale] * registers.read(sib.index, RegisterBuffer::Double))
  } else { return 0; }
}

uint16_t[8] Core::InstrDevelopmentTools::resolveImmediateArguments(InstrPackage& package) {
  uint8_t buffer = Core::InstrDevelopmentTools::datatype2buffer[package.instr.datatype][0];
  uint16_t args[4] = {
    fetch(),                         // 1 => Word
    (buffer - 1) == 2 ? fetch() : 0, // 2 => Double
    (buffer - 1) == 3 ? fetch() : 0, // 3 => Quad
    (buffer - 1) == 3 ? fetch() : 0, // 3 => Quad
  };
  return args;
}

auto Core::InstrDevelopmentTools::formatArgument(DataType typing, unsigned int argument, bool selectSecond) {
	switch(typing) {
		case DataType::INT16: return static_cast<int16_t>(argument);
		case DataType::INT32: return static_cast<int32_t>(argument);
		case DataType::INT64: return static_cast<int64_t>(argument);
		case DataType::UINT16: return static_cast<uint16_t>(argument);
		case DataType::UINT32: return static_cast<uint32_t>(argument);
		case DataType::UINT64: return static_cast<uint64_t>(argument);
		case DataType::FLOAT32: return static_cast<float32_t>(argument);
		case DataType::FLOAT64: return static_cast<float64_t>(argument);
		case DataType::FLOAT32_INT32: return selectSecond ? static_cast<int32_t>(argument) : static_cast<float32_t>(argument);
		case DataType::FLOAT64_INT32: return selectSecond ? static_cast<int32_t>(argument) : static_cast<float64_t>(argument);
	}
}

bool Core::InstrDevelopmentTools::checkNecessaryCPL(CPL privilege) {
  if (privilege == CPL::Kernel) { return registers.isKernelMode(); }
  else if (privilege == CPL::User) { return !registers.isKernelMode(); }
  else { return false; }
}

Core& Core::shutdown() {
  this->halted = true;
  delay(50);
  return *this;
}

Core& Core::sleep() {
  setLowPower();
  return *this;
}

Core& Core::reset() {
  return Core();
}

auto* Core::dump() {

}

uint16_t Core::fetch(bool instr) {
  clock();
  uint64_t ip = registers.IP * 2;
  /*
		LITTLE ENDIAN BE LIKE:
		--------------------
		ADD AX, [0x000A100F]
		--------------------
		00000010 00000000 (0x0002: ADD-16)
		00000001 00000000 (0x0001: AX)
		00001000 00001111 (0x100F: MEM_LOW)
		00000000 00001010 (0x000A: MEM_HIGH)
	*/
  uint16_t word = instr ? cache.L1.readInstr(ip, cache.policies.fifo) : cache.L1.readData(ip, cache.policies.minWeight);
  registers.IP++;
  return word;
}

/*
ModR/M: [Subcode, Mode, RMS, RMD]
Format:     Subcode:  Mode:     Source:       Destination:  Operand_0:    Operand_1:    Note:
  NOTHING   *void     *void     *void         *void         *void         *void         No se usa ModS/D
  IMM       *void     *void     *void         *void         Inmediato     *void         No se usa ModS/D
  REG       ...       ...       ...           Registro      *void         *void         ...
  MEM       ...       0x0       ...           ...           [Inmediato]   *void         Sin Desplazamiento
  MEM       ...       0x1       ...           ...           [Inmediato]   *void         Desplazamiento 16 Bits
  MEM       ...       0x2       ...           ...           [Inmediato]   *void         Desplazamiento 32 Bits
  MEM       ...       0x3       ...           [Registro]    *void         *void         Sin Desplazamiento
  MEM       ...       0x4       ...           [Registro]    *void         *void         Desplazamiento 16 Bits
  MEM       ...       0x5       ...           [Registro]    *void         *void         Desplazamiento 32 Bits
  MEM       ...       0x6       ...           ...           *void         *void         SIB con Registro Indice
  MEM       ...       0x7       ...           ...           ...           *void         SIB con Indice Inmediato
  IMM_IMM   *void     *void     *void         *void         Inmediato     Inmediato     No se usa ModS/D
  REG_IMM   ...       ...       ...           Registro      Inmediato     *void         ...
  REG_REG   ...       ...       Registro      Registro      *void         *void         ...
  REG_MEM   ...       0x0       ...           Registro      [Inmediato]   *void         Sin Desplazamiento
  REG_MEM   ...       0x1       ...           Registro      [Inmediato]   *void         Desplazamiento 16 Bits
  REG_MEM   ...       0x2       ...           Registro      [Inmediato]   *void         Desplazamiento 32 Bits
  REG_MEM   ...       0x3       [Registro]    Registro      *void         *void         Sin Desplazamiento
  REG_MEM   ...       0x4       [Registro]    Registro      *void         *void         Desplazamiento 16 Bits
  REG_MEM   ...       0x5       [Registro]    Registro      *void         *void         Desplazamiento 32 Bits
  REG_MEM   ...       0x6       ...           Registro      *void         *void         SIB con Registro Indice
  REG_MEM   ...       0x7       ...           Registro      *void         *void         SIB con Indice Inmediato
  MEM_IMM   ...       0x0       ...           ...           Inmediato     [Inmediato]   Sin Desplazamiento
  MEM_IMM   ...       0x1       ...           ...           Inmediato     [Inmediato]   Desplazamiento 16 Bits
  MEM_IMM   ...       0x2       ...           ...           Inmediato     [Inmediato]   Desplazamiento 32 Bits
  MEM_IMM   ...       0x3       ...           [Registro]    Inmediato     *void         Sin Desplazamiento
  MEM_IMM   ...       0x4       ...           [Registro]    Inmediato     *void         Desplazamiento 16 Bits
  MEM_IMM   ...       0x5       ...           [Registro]    Inmediato     *void         Desplazamiento 32 Bits
  MEM_IMM   ...       0x6       ...           ...           Inmediato     *void         SIB con Registro Indice
  MEM_IMM   ...       0x7       ...           ...           Inmediato     *void         SIB con Indice Inmediato
  MEM_REG   ...       0x0       ...           Registro      [Inmediato]   *void         Sin Desplazamiento
  MEM_REG   ...       0x1       ...           Registro      [Inmediato]   *void         Desplazamiento 16 Bits
  MEM_REG   ...       0x2       ...           Registro      [Inmediato]   *void         Desplazamiento 32 Bits
  MEM_REG   ...       0x3       [Registro]    Registro      *void         *void         Sin Desplazamiento
  MEM_REG   ...       0x4       [Registro]    Registro      *void         *void         Desplazamiento 16 Bits
  MEM_REG   ...       0x5       [Registro]    Registro      *void         *void         Desplazamiento 32 Bits
  MEM_REG   ...       0x6       ...           Registro      *void         *void         SIB con Registro Indice
  MEM_REG   ...       0x7       ...           Registro      *void         *void         SIB con Indice Inmediato
*/

InstrPackage Core::decode(uint16_t opcode) {
  InstrPackage package;

  package.reset();
  package.instr.raw = opcode;

  const uint8_t necessaryPrivilege = ISA.get(package.instr.opcode).privilege;
  if (necessaryPrivilege == CPL::Kernel !& registers.isKernelMode()) {
    stderr::RequiredPermits();
    package.instr.raw = 0x0; // NOP
    return package;
  }

  const uint8_t format = ISA.get(package.instr.opcode).format;
  auto doModrm = [this]() -> void {
    switch (package.modrm.mode) {
      case 0x0: // Sin Desplazamiento : Source << Memoria
        package.operand.arg0 = fetch();
        package.operand.arg1 = fetch();
        InstrDevelopmentTools::_offsetArguments = 2;
        break;
      case 0x1: // Desplazamiento 16 Bits : Source << Memoria
        package.disp = fetch();
        package.operand.arg0 = fetch();
        package.operand.arg1 = fetch();
        InstrDevelopmentTools::_offsetArguments = 2;
        break;
      case 0x2: // Desplazamiento 32 Bits : Source << Memoria
        package.disp = Utils::i16toi32(fetch(), fetch());
        package.operand.arg0 = fetch();
        package.operand.arg1 = fetch();
        InstrDevelopmentTools::_offsetArguments = 2;
        break;
      case 0x3: // Sin Desplazamiento : Source << Registro
        InstrDevelopmentTools::_offsetArguments = 2;
        break;
      case 0x4: // Desplazamiento 16 Bits : Source << Registro
        package.disp = fetch();
        InstrDevelopmentTools::_offsetArguments = 0;
        break;
      case 0x5: // Desplazamiento 32 Bits : Source << Registro
        package.disp = Utils::i16toi32(fetch(), fetch());
        InstrDevelopmentTools::_offsetArguments = 0;
      case 0x6: // SIB Indice con valor de Registro
      case 0x7: // SIB Indice con valor Inmediato
        package.sib.raw = fetch();
        InstrDevelopmentTools::_offsetArguments = 0;
        break;
      default:
        InstrDevelopmentTools::_offsetArguments = 0;
        break;
    }
  }

  switch (format) {
    case OperandFormat::NOTHING:
      break;
    case OperandFormat::REG_IMM:
    case OperandFormat::REG_REG:
    case OperandFormat::REG_MEM:
    case OperandFormat::MEM_IMM:
    case OperandFormat::MEM_REG:
      package.modrm.raw = fetch();
      doModrm();
      break;
    case OperandFormat::IMM:
      break;
    case OperandFormat::REG:
    case OperandFormat::MEM:
      package.modrm.raw = fetch();
      doModrm();
      break;
    default:
      stderr::InstructionFormatUnrecognized();
      package.instr.opcode = 0x0; // NOP
      break;
  }

  return package;
}

Core& Core::execute(InstrPackage package) {
    uint16_t opcode = package.instr.opcode;

    if (0 <= opcode <= this->lenISA) {
      const InstrOfISA& entry = ISA.get(opcode);
      (this->*entry.func)(package, entry.format, entry.privilege);
    } else {
      stderr::InstructionNotFound();
    }

    return *this;
}

void Core::run() {
  // fill() : Rellena el Pipeline con la primera instrucción.
  if (!halted) decode(fetch());

  while (!halted) {
    // Execute
    if (pipeline[2].valid) execute(pipeline[2].instrPackage);

    // Decode
    if (pipeline[1].valid) {
      pipeline[2].instrPackage = decode(pipeline[1].rawOpcode);
      pipeline[2].valid = true;
    } else {
      pipeline[2].valid = false;
    }

    // Fetch
    pipeline[1].rawOpcode = fetch();
    pipeline[1].valid = true;

    // Next Instr...
    pipeline[2] = pipeline[1];
    pipeline[1] = pipeline[0];
    pipeline[0].valid = false;
  }
}
