
#include "registers.h"
#include "io.h"

bool Registers::isKernelMode() {
  return CS == 0;
}

/** [-] FUNCTION: __validate__();
* @param regID [uint8_t; Register ID]
* @param buffer [Byte High, Byte Low, Word, Double, Quad]
* @return unsigned int;
*
** [*] IDREGISTER:
*  ----------------------------------------------------------
* | ID | SYMBOL |      NAME      |       BUFFERS       | CPL |
* |----|--------|----------------|---------------------|-----|
* |  0 |     AX | Acumulator     |  8l, 8h, 16, 32, 64 |   3 |
* |  1 |     BX | Base           |  8l, 8h, 16, 32, 64 |   3 |
* |  2 |     CX | Counter        |  8l, 8h, 16, 32, 64 |   3 |
* |  3 |     DX | Data           |  8l, 8h, 16, 32, 64 |   3 |
*  ----------------------------------------------------------
*
** [*] NOTE:
* - CPL (Current Privilege Protocol) is used to access to the registers of system... CPL 0 is equal to Kernel Mode Necessary & CPL 3 is equal to User Mode or Kernel Mode Necessary. 
*
** [!] ERRORS:
* - stderr.RequiredPermits  : "Any Registers require permits to access it.""
* - stderr.RegisterNotFound : "If you pass a invalid /regID/ then it'll be called the RegisterNotFound Error."
*/
unsigned int& Registers::validate(uint8_t regID, RegisterBuffer buffer) {
  unsigned int& currentRegister;
  switch (regID) {
    case 0:
      currentRegister = {Registers::AX.r8l, Registers::AX.r8h, Registers::AX.r16, Registers::AX.r32, Registers::AX.r64};
      break;
    case 1:
      currentRegister = {Registers::BX.r8l, Registers::BX.r8h, Registers::BX.r16, Registers::BX.r32, Registers::BX.r64};
      break;
    case 2:
      currentRegister = {Registers::CX.r8l, Registers::CX.r8h, Registers::CX.r16, Registers::CX.r32, Registers::CX.r64};
      break;
    case 3:
      currentRegister = {Registers::DX.r8l, Registers::DX.r8h, Registers::DX.r16, Registers::DX.r32, Registers::DX.r64};
      break;
    case 4:
      currentRegister = {Registers::SI.r8l, Registers::SI.r8h, Registers::SI.r16, Registers::SI.r32, Registers::SI.r64};
      break;
    case 5:
      currentRegister = {Registers::DI.r8l, Registers::DI.r8h, Registers::DI.r16, Registers::DI.r32, Registers::DI.r64};
      break;
    case 6:
      currentRegister = {Registers::BP.r8l, Registers::BP.r8h, Registers::BP.r16, Registers::BP.r32, Registers::BP.r64};
      break;
    case 7:
      currentRegister = {Registers::ESP.r8l, Registers::ESP.r8h, Registers::ESP.r16, Registers::ESP.r32, Registers::ESP.r64};
      break;
    case 8:
      return Registers::R8;
    case 9:
      return Registers::R9;
    case 10:
      return Registers::R10;
    case 11:
      return Registers::R11;
    case 12:
      return Registers::R12;
    case 13:
      return Registers::R13;
    case 14:
      return Registers::R14;
    case 15:
      return Registers::R15;
    case 16:
      return Registers::FLAGS;
    case 17:
      return Registers::XMM0;
    case 18:
      return Registers::XMM1;
    case 19:
      return Registers::XMM2;
    case 20:
      return Registers::XMM0;
    case 21:
      return isKernelMode() ? Registers::CS : stderr.RequiredPermits();
    case 22:
      return Registers::DS;
    case 23:
      return isKernelMode() ? Registers::SS : stderr.RequiredPermits();
    case 24:
      return Registers::ES;
    case 25:
      return Registers::FS;
    case 26:
      return Registers::GS;
    case 27:
      return isKernelMode() ? Registers::CR0 : stderr.RequiredPermits();
    case 28:
      return isKernelMode() ? 0x0 : stderr.RequiredPermits();
    case 29:
      return isKernelMode() ? Registers::CR2 : stderr.RequiredPermits();
    case 30:
      return isKernelMode() ? Registers::CR3 : stderr.RequiredPermits();
    case 31:
      return isKernelMode() ? Registers::CR4 : stderr.RequiredPermits();
    case 32:
      return isKernelMode() ? Registers::DR0 : stderr.RequiredPermits();
    case 33:
      return isKernelMode() ? Registers::DR1 : stderr.RequiredPermits();
    case 34:
      return isKernelMode() ? Registers::DR2 : stderr.RequiredPermits();
    case 35:
      return isKernelMode() ? Registers::DR3 : stderr.RequiredPermits();
    case 36:
      return isKernelMode() ? 0x0 : stderr.RequiredPermits();
    case 37:
      return isKernelMode() ? 0x0 : stderr.RequiredPermits();
    case 38:
      return isKernelMode() ? Registers::DR6 : stderr.RequiredPermits();
    case 39:
      return isKernelMode() ? Registers::DR7 : stderr.RequiredPermits();
    default: 
      stderr.RegisterNotFound();
  }
  return currentRegister[buffer];
}

Registers& Registers::write(uint8_t regID, unsigned int data, RegisterBuffer buffer) {
  unsigned int& currentRegister = validate(regID, buffer);
  currentRegister = data;
  return *this;
}

unsigned int Registers::read(uint8_t regID, RegisterBuffer buffer) {
  unsigned int currentRegister = validate(regID, buffer);
  return currentRegister;
}
