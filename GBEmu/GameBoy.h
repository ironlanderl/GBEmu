#pragma once
#include <cstdint>
#include <fstream>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <nlohmann/json.hpp>
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
	bool IME;

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
	uint8_t PROHIBITED[0xFEFF - 0xFEA0+1];
	uint8_t IE;
	void writeBusUnrestricted(uint8_t value, uint16_t address);
	void writeBus(uint8_t value, uint16_t address);
	uint8_t getBus(uint16_t address);
	void loadRom(char cart[], std::streamsize fileSize);

	// STACK STUFF
	void PUSH_STACK_8BIT(uint8_t value);
	void PUSH_STACK_16BIT(uint16_t value);
	uint8_t POP_STACK_8BIT();
	uint16_t POP_STACK_16BIT();

	// MAPPER STUFF
	void handleMapperWrites(uint8_t value, uint16_t address);
	uint8_t handleMapperReads(uint16_t address);

	struct MBC1_SETTINGS
	{
		bool RAM_ENABLED;
		uint8_t ROM_BANK;
		uint8_t RAM_BANK;
		uint8_t BANK_MODE;
	} mbc1_settings;
	


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
	uint16_t fuse_two_bytes(uint8_t a, uint8_t b);

	// Opcodes
	void opcode_nop();
	void dec_8bit_register(uint8_t &reg);
	void dec_16bit_register(uint8_t& reg_a, uint8_t& reg_b);
	void dec_16bit_register(uint16_t& reg);
	void inc_8bit_register(uint8_t& reg);
	void inc_16bit_register(uint8_t& reg_a, uint8_t& reg_b);
	void inc_16bit_register(uint16_t& reg);
	void dec_at_address();
	void inc_at_address();
	void ld_8bit_register(uint8_t& dest, uint8_t source);
	void ld_8bit_value(uint8_t& dest, uint8_t value);
	void ld_16bit_value(uint8_t& dest_a, uint8_t& dest_b, uint16_t value);
	void ld_16bit_value(uint16_t &dest, uint16_t value);
	void ld_a_to_address();
	void ld_a_to_wram_offset();
	void ld_wram_offset_to_a();
	void ld_8bit_value_from_ram(uint8_t& dest, uint16_t source);
	void add_a_u8();
	void add_a_reg(uint8_t reg);
	void sub_a_u8();
	void sub_a_reg(uint8_t reg);
	void and_a_reg(uint8_t reg);
	void and_a_u8();
	void and_a_hl();
	void cp_a_reg(uint8_t reg);
	void cp_a_u8();
	void cp_a_hl();
	void adc_a_reg(uint8_t reg);
	void adc_a_u8();
	void adc_a_hl();
	void pop_stack(uint8_t& reg_a, uint8_t& reg_b);
	void push_stack(uint8_t& reg_a, uint8_t& reg_b);
	void call_u16();
	void jr(bool condition);
	void jr_i8();
	void jp(bool condition);
	void jp_u16();
	void jp_hl();
	void ret();
	void ret(bool condition);
	void reti();
	void rst_vector(uint8_t vector);
};

