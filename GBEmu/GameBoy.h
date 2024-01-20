#pragma once
#include <cstdint>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <SFML/Config.hpp>

enum PossibleStatus
{
	STOPPED,
	RUNNING,
	PAUSED
};

class GameBoy
{
public:
	// CPU REGISTERS
	uint8_t A;
	uint8_t B;
	uint8_t C;
	uint8_t D;
	uint8_t E;
	uint8_t H;
	uint8_t L;
	uint16_t PC;
	uint16_t SP;
	uint16_t ZeroFlag;
	uint16_t NegativeFlag;
	uint16_t HalfCarry;
	uint16_t Carry;

	void setAF(uint16_t value);
	void setBC(uint16_t value);
	void setDE(uint16_t value);
	void setHL(uint16_t value);

	uint16_t getAF();
	uint16_t getBC();
	uint16_t getDE();
	uint16_t getHL();

	// MEMORY AND IO
	uint8_t ROM[0x80000]; // Dovrebbero bastare
	uint8_t VRAM[0x2000];
	uint8_t WRAM0[0x2000];
	uint8_t WRAM1[0x2000];
	uint8_t OAM[0xFE9F - 0xFE00 + 1];
	uint8_t HRAM[0xFFFE - 0xFF80 + 1];
	uint8_t IE;
	void writeBus(uint8_t value, uint16_t address);
	uint8_t getBus(uint16_t address);
	void loadRom(char cart[], std::streamsize fileSize);

	// BASIC
	PossibleStatus status = STOPPED;
	uint64_t m_cycles;
	uint64_t t_cycles;
	void add_m_cycles(uint8_t cycles_to_add);
	void add_t_cycles(uint8_t cycles_to_add);
	void advanceStep();

	// UTILS
	bool isInsideInterval(uint16_t value, uint16_t bottom_bound, uint16_t upper_bound);
	uint16_t get_next_two_bytes(uint16_t address);
	uint8_t get_next_byte(uint16_t address);
};

