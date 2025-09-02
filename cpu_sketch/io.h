
#pragma once
#include "esp32-hal.h"
#include <Arduino.h>
#include <SPI.h>
#include "utils.h"
#include "core.h"

/*
  RGB LED:
    Kernel Mode : #322501
    User Mode   : #300000
*/

// GPIO - FEATURES
#define CLOCK_PIN 2 // On-Board LED
#define SLEEP_PIN 4
#define MODE_PIN 48 // On-Board RGB LED

// SPI2 - BUSES
#define BUSES_PIN_SCK 5
#define BUSES_PIN_MISO 6
#define BUSES_PIN_MOSI 7
#define GPU_PIN_CS 15
#define CHIPSET_PIN_CS 16
#define PIC_PIN_CS 17
#define UNUSEDBUS_PIN_CS 18

// SPI3 - MEMORY
#define MEMORY_PIN_SCK 9
#define MEMORY_PIN_MISO 10
#define MEMORY_PIN_MOSI 11
#define MEMORY_PIN_CS 12

// UART1 - USB
#define USB0_PIN_DNEG 19 // On-board USB D-
#define USB0_PIN_DPOS 20 // On-board USB D+
#define USB1_PIN_DNEG 13
#define USB1_PIN_DPOS 14

// INT - INTERRUPTS
#define INTA_PIN 38 // Interrupt Answer
#define INTR_PIN 39 // Interrupt Request
#define NMI_PIN 40 // Non-Maskable Interrupt

// UART2 - DUMPER COPROCESSOR
#define DUMPER_PIN_TX 41
#define DUMPER_PIN_RX 42

// UART0 - BOOTER
#define BOOTER_PIN_TX 43
#define BOOTER_PIN_RX 44

// Code: 225519833; NIP: evelyn0809, evelyno8o9
// Code: 225520297; NIP Mail: kareli2010*; NIP Leo: Kareli2010

void beginIO() {
  // Starting Addr-Data Buses & Memory "DDR" Buses
  SPI.begin(BUSES_PIN_SCK, BUSES_PIN_MISO, BUSES_PIN_MOSI);
  digitalWrite(GPU_PIN_CS, 1); digitalWrite(CHIPSET_PIN_CS, 1); digitalWrite(PIC_PIN_CS, 1);
  SPI1.begin(MEMORY_PIN_SCK, MEMORY_PIN_MISO, MEMORY_PIN_MOSI);
  digitalWrite(MEMORY_PIN_CS, 1); digitalWrite(DUMPER_PIN_CS, 1);

  // Starting USB-UART Pines
  Serial.begin(USB1_PIN_DNEG, USB1_PIN_DPOS);
  Serial1.begin(USB2_PIN_DNEG, USB2_PIN_DPOS);
  
  // Starting IO Pines
  pinMode(CLOCK_PIN, INPUT);
  pinMode(SLEEP_PIN, INPUT);
  pinMode(NMI_PIN, INPUT); pinMode(INTR_PIN, INPUT); pinMode(INTA_PIN, OUTPUT);

}

void setLowPower() {
  
}

bool waitToClock() {
  while (digitalRead(CLOCK_PIN) == 1);
  digitalWrite(CLOCK_PIN, 1);
  while (digitalRead(CLOCK_PIN) == 0);
  digitalWrite(CLOCK_PIN, 0);
  return true;
}

class Trap {
  Trap(Cache& cache, Registers& registers, uint8_t isr) {
    this->cache = cache;
    this->registers = registers;
    this->osr = isr;
  }

  void raise() {
    uint32_t intPointer = registers.IDTR + isr * 2;
    uint32_t taskPointer = registers.TR + (registers.read(21) & 0x3) * 2; // .read(21) -> CS
    cache.L1.write(taskPointer, cache.L1.readData(registers.read(7) & 0x00FF)); // .read() -> SP
    cache.L1.write(taskPointer, cache.L1.readData(registers.read(7) & 0xFF00 >> 8)); // .read() -> SP
    registers.IP = cache.L1.readData(intPointer);
  }
};

class Fault {
  void raise() {}
};

class Abort {
  char errorMessage[];

  Abort(char* message) {
    this->errorMessage = message;
  }

  Abort& raise() {
    // Aqui se enviara los bits de un pantallaso azul de la BIOS que esta
    // previamente programado en el CPU (nada que ver a la realidad xD)
    // hacia el GPU para que lo muestre en pantalla
    SPI.send();
    return *this;
  }
};

namespace stdint {
  Abort InstructionNotFound = Abort(char* err);

  void InstructionFormatUnrecognized(char* err) {}

  void RegisterNotFound() {}

  /** [!] ERROR:
  * CS: CPL != 0
  *  \-[ Current Permission Level isn't in Kernel Mode - CPL_0 ]
  */
  void RequiredPermits() {}

  void PinModeNotAvailable() {}

  void ChipsetNotResponse() {}

};

enum class PinesGPIO : uint8_t { GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5, GPIO6, GPIO7, GPIO8, GPIO9 };
enum class PinModeGPIO : uint8_t { Digital, Analog, PWM };
uint8_t GPIO2IO[3][10] = { // Virtual GPIO -> Atmega385P IO
  { 5, 6, 7, 8, 9, 10, 12, 13, 19, 20 },
  { 19, 20, 21, 22, 23, 24, 25, 26, 0, 0 },
  { 6, 8, 9, 12, 13, 0, 0, 0, 0, 0 } // 0 => Pin NO Existe/NO es Posible
};

namespace stdin {
  uint16_t input(PinesGPIO pin, PinModeGPIO mode = PinModeGPIO::Digital) {
    uint8_t IOPin = GPIO2IO[static_cast<uint8_t>(mode)][static_cast<uint8_t>(pin)];

    if (IOPin == 0) { stderr::PinModeNotAvailable(); }
    uint8_t attempt = 0;

    /** [*] NOTE:
    * 0x0 : NOP             -> void*
    * 0x1 : HLT             -> void*
    * 0x2 : IN              -> src
    * 0x3 : IN  [Digital]   -> IO
    * 0x4 : IN  [Analog]    -> IO
    * 0x5 : IN  [PWM]       -> IO
    * 0x6 : OUT             -> src, data
    * 0x7 : OUT [Digital]   -> IO
    * 0x8 : OUT [Analog]    -> IO
    * 0x9 : OUT [PWM]       -> IO
    */
    Serial.write(mode + 3); // Command: Pin Mode
    Serial.write(IOPin);    // Param:   Pin IO

    while (attempt < 256) {
      if (Serial.available() >= 2) {
        int16_t response = (Utils::i8toi16(Serial.read(), Serial.read()));
        return (uint8_t)response;
      }
      attempt++;
      delay(1);
    }
    stderr::ChipsetNotResponse();
  }

  uint8_t memory(uint64_t addr) {
    
  }
  void storage() {}
};

namespace stdout {
  void output() {}

  void memory(uint64_t addr, uint8_t data) {
    
  }
  void storage() {}
};
