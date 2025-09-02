
#pragma once
#include <Arduino.h>
#include <math.h>
#include "registers.h"
#include "cache.h"
#include "isa.h"
#include "io.h"

using float32_t = float; using float64_t = double;

// Tecnicamente no hay nada de malo, ¿no?
// Pero como que algo no cuadra, ¿no?
using Buffer = RegisterBuffer;

#define DEFAULT_SIB 4160 // SIB : 0 + (1 * 1) -> 0 0 01 000001 000000 (4160)

/* [!] NOTA QUE SE ME OLVIDA:
	Family				: {family: 16 bits}
	Instruction 	: {type: 4 bits; opcode: 12 bits}
		ModR/M 			: {n/a: 1 bits; mode: 3 bits; reg: 6 bits; r/m: 6 bits}
			SIB				: {n/a: 1 bits; disp16: 1 bits; scale: 2 bits; index: 6 bits; base: 6 bits}
			Disp16		: {disp: 16 bits}
			Disp32		: {disp0: 16 bits}, {disp1: 16 bits}
		Operands		: {imm: 16 bits}...

	ADD-16 [REG + disp16], IMM =>
		family: {0000000000000000}, instr: {0100 000000000010}, modr/m: {00 00 000000 xxxxxx}, disp16: {xxxxxxxxxxxxxxxx}, op0: {xxxxxxxxxxxxxxxx}

	ADD-16 [AX + 1], 0xF
				+------------------+------------------+------------------+------------------+
	0000h | 0000000000000010 | 0001000000000010 | 0000000000000001 | 0000000000001111 | 0008h
				+------------------+------------------+------------------+------------------+
				 ....Instruction... ......ModR/M...... ...Displacement... ....Operand_1.....
*/

enum class DataType : uint16_t { 
	INT16, INT32, INT64,
	UINT16, UINT32, UINT64,
	FLOAT32, FLOAT64,
	FLOAT32_INT32, FLOAT64_INT32,	
};

enum class OperandFormat : uint8_t {
	NOTHING,
	REG_IMM, REG_REG, REG_MEM,
	MEM_IMM, MEM_REG,
	IMM,
	REG,
	MEM
};

struct InstrPackage {
	union {
		struct {
			uint16_t opcode 		:	12;	// 12 Bits
			DataType datatype		: 4;	// 4 Bits
		};
		uint16_t raw;
	} instr;
	union {
		struct {
			uint16_t rm					: 6; // 6 Bits
			uint16_t reg				: 6; // 6 Bits
			uint16_t mode				: 3; // 3 Bits
			uint16_t subcode		: 1; // 1 Bits
		};
		uint16_t raw;
	} modrm;
	union {
		struct {
			uint16_t base				: 6; // 6 Bits
			uint16_t index			: 6; // 6 Bits
			uint16_t scale			: 2; // 2 Bits
			uint16_t offset 		: 1; // 1 Bits
			uint16_t subcode 		: 1; // 1 Bits
		};
		uint16_t raw;
	} sib;
	uint32_t disp; // 16-32 Bits
	union {
		struct {
			uint16_t arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7;
		};
		uint16_t args[8];
	} operand;

	void reset() {
		instr.raw = 0;
		modrm.raw = 0;
		sib.raw = DEFAULT_SIB;
		disp = 0;
		for (uint8_t i = 0; i < 8; i++) operand.args[i] = 0;
	}

	InstrPackage& fillArguments(uint16_t arguments[8]) {
		for (uint_t i = 0; i < 4; i++) operand.args[i] = arguments[i];
		return *this;
	}
};

struct InstrOfISA {
  void (Core::*func)(InstrPackage, OperandFormat, CPL);
  OperandFormat format;
	CPL privilege;
};

struct Pipeline {
	uint16_t rawOpcode = 0;
	InstrPackage instrPackage = {};
	bool valid = false;
};

enum class CPL : uint8_t { Kernel = 0, User = 3 }

class Core {
private:
	Pipeline pipeline[3];

	Cache cache = Cache();
	Registers registers = Registers();

//##### x86 ##################################################
void _nop(InstrPackage package, OperandFormat format, CPL privilege);
void _hlt(InstrPackage package, OperandFormat format, CPL privilege);
void _rand(InstrPackage package, OperandFormat format, CPL privilege);
void _add(InstrPackage package, OperandFormat format, CPL privilege);
void _adc(InstrPackage package, OperandFormat format, CPL privilege);
void _sub(InstrPackage package, OperandFormat format, CPL privilege);
void _sbb(InstrPackage package, OperandFormat format, CPL privilege);
void _mul(InstrPackage package, OperandFormat format, CPL privilege);
void _imul(InstrPackage package, OperandFormat format, CPL privilege);
void _div(InstrPackage package, OperandFormat format, CPL privilege);
//##### x86 ##################################################

	static struct InstrDevelopmentTools {
		uint8_t _offsetArguments = 0;

		uint32_t resolveSIB(InstrPackage::sib sib, InstrPackage::modrm modrm);
		uint16_t* resolveImmediateArguments(InstrPackage::operand::args arguments);
			
		/*
			enum class DataType : uint16_t { 
				INT16, INT32, INT64,
				UINT16, UINT32, UINT64,
				FLOAT32, FLOAT64,
				FLOAT32_INT32, FLOAT64_INT32,	
			};
			enum class RegisterBuffer { ByteLow, ByteHigh, Word, Double, Quad };
			using Buffer = RegisterBuffer;
		*/
		const uint8_t datatype2buffer[10][2] = {
			{ Buffer::Word,	Buffer::Word },
			{ Buffer::Double, Buffer::Double },
			{ Buffer::Quad,	Buffer::Quad },
			{ Buffer::Word,	Buffer::Word },
			{ Buffer::Double,	Buffer::Double },
			{ Buffer::Quad,	Buffer::Quad },
			{ Buffer::Double,	Buffer::Double},
			{ Buffer::Quad,	Buffer::Quad },
			{ Buffer::Double,	Buffer::Double },
			{ Buffer::Quad,	Buffer::Double },
		};

		auto formatArgument(DataType typing, unsigned int argument, bool selectSecond = false);

		bool checkNecessaryCPL(CPL privilege = CPL::Kernel);
	};

public:
	bool halted = false;
	
	Core();

	Core& shutdown();
	Core& suspend();
	Core& reset();
	auto* dump();

	uint16_t fetch(bool instr = false);
	InstrPackage decode(uint16_t opcode);

	Core& execute(InstrPackage pacakge);
	void run();
};
