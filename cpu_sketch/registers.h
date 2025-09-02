
#pragma once
#include <Arduino.h>
#include "utils.h"

union Register64 {
    uint64_t r64;
    uint32_t r32;
    uint16_t r16;
    uint8_t r8h;
    uint8_t r8l;
};
union Register32 {
    uint32_t r32;
    uint16_t r16;
    uint8_t r8h;
    uint8_t r8l;
};
union Register16 {
    uint16_t r16;
    uint8_t r8h;
    uint8_t r8l;
};

enum class RegisterBuffer { ByteLow, ByteHigh, Word, Double, Quad };

class Registers {
private:
    // General Registers
    Register64 AX;
    Register64 BX;
    Register64 CX;
    Register64 DX;
    Register64 SI;
    Register64 DI;
    Register64 BP;
    Register64 ESP; // Stack Point -> Hay un macro que no me deja poner SP
    uint16_t R8;
    uint16_t R9;
    uint16_t R10;
    uint16_t R11;
    uint16_t R12;
    uint16_t R13;
    uint16_t R14;
    uint16_t R15;

    // Registros de Segmentación
    uint16_t CS; // Apunta a un Descriptor de Codigo en GDT (Global Description Table) o LDT (Local Description Table)
    uint16_t DS; // Apunta a un Descriptor de Datos en GDT o LDT
    uint16_t SS; // Apunta a un Descriptor de Segmento de Pila en GDT o LDT
    uint16_t ES; // Apunta a un Descriptor de Datos Extras de Pila en GDT o LDT
    uint16_t FS; // Apunta a un Descriptor de Datos Especiales
    uint16_t GS; // Apunta a un Descriptor de Datos Especiales

    // SIMD Registers
    Register64 XMM0;
    Register64 XMM1;
    Register64 XMM2;

    // Flags Register
    union {
        uint8_t r8h;
        bool IF;
        bool DF;
        bool PF;
        bool HF;
        bool OF;
        bool SF;
        bool CF;
        bool ZF;
    } FLAGS; 

    // Control Registers [Features Unables]
    union {
        bool PE; // Modo Protegido
        bool PG; // Paginación Habilitada...
    } CR0;
    uint32_t CR2; // Dirección de Última Falla de Pagina (Page Fault)
    uint32_t CR3; // Dirección Base de Tabla de Paginas (PDBR)
    uint32_t CR4; // Control Avanzado del CPU

    // Debug Registers
    uint32_t DR0, DR1, DR2, DR3; // Direcciónes de Breakpoins (DR)
    uint32_t DR6; // Indica Breakpoints Activados (DSR)
    uint32_t DR7; // Controla la Activacion de Breakpoints (DCR)

public:
    uint32_t IP = 0; // Instruction Pointer
    uint32_t IDTR = 0; // Interrupt Descriptor Table Register
    uint32_t TR = 0; // Task Register

    bool isKernelMode();
    auto validate(uint8_t regID, RegisterBuffer buffer);

    Registers& write(uint8_t regID, unsigned int data = 0, RegisterBuffer buffer = RegisterBuffer::Word);
    auto read(uint8_t regID, RegisterBuffer buffer = RegisterBuffer::Word);
};