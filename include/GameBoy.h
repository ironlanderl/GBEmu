#pragma once
#include <cstdint>
#include <fstream>
#include <cstring>
#include <functional>
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
	bool IME;
	bool halted;

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
	uint8_t PROHIBITED[0xFEFF - 0xFEA0 + 1];
	uint8_t IE;
	void writeBusUnrestricted(uint8_t value, uint16_t address);
	uint8_t getBusUnrestriced(uint16_t address);
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
	void handle_interrupts();
	void run_tick();
	void execute_step();
	

	// Interrupt stuff
	uint8_t getIE();
	uint8_t getIF();

	// UTILS
	bool isInsideInterval(uint16_t value, uint16_t bottom_bound, uint16_t upper_bound);
	uint16_t get_next_two_bytes(uint16_t address);
	uint8_t get_next_byte(uint16_t address);
	uint16_t fuse_two_bytes(uint8_t a, uint8_t b);
	void split_two_bytes(uint16_t value, uint8_t& a, uint8_t& b);
	void increment_16bit_register(uint8_t& reg_a, uint8_t& reg_b);
	void decrement_16bit_register(uint8_t& reg_a, uint8_t& reg_b);

	// Opcodes
	void opcode_nop();
	void dec_8bit_register(uint8_t& reg);
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
	void ld_16bit_value(uint16_t& dest, uint16_t value);
	void ld_mem_8bit();
	void ld_to_mem(uint8_t& reg_a, uint8_t& reg_b, uint8_t value);
	void ld_u8_to_mem();
	void ld_sp_to_u16();
	void ld_a_to_address();
	void ld_a_to_wram_offset();
	void ld_wram_offset_to_a();
	void ld_8bit_value_from_ram(uint8_t& dest, uint16_t source);
	void ld_hl_sp_i8();
	void ld_hram_c_a();
	void ld_a_hram_c();
	void add_a_u8();
	void add_a_reg(uint8_t reg);
	void add_a_address();
	void add_hl_16bit(uint8_t reg_a, uint8_t reg_b);
	void add_sp_i8();
	void sub_a_u8();
	void sub_a_reg(uint8_t reg);
	void sub_a_address();
	void and_a_reg(uint8_t reg);
	void and_a_u8();
	void and_a_hl();
	void or_a_reg(uint8_t reg);
	void or_a_u8();
	void or_a_hl();
	void xor_a_reg(uint8_t reg);
	void xor_a_u8();
	void xor_a_hl();
	void cp_a_reg(uint8_t reg);
	void cp_a_u8();
	void cp_a_hl();
	void adc_a_reg(uint8_t reg);
	void adc_a_u8();
	void adc_a_hl();
	void sbc_a_reg(uint8_t reg);
	void sbc_a_u8();
	void sbc_a_hl();
	void pop_stack(uint8_t& reg_a, uint8_t& reg_b);
	void push_stack(uint8_t& reg_a, uint8_t& reg_b);
	void call(bool condition);
	void call_u16();
	void jr(bool condition);
	void jr_i8();
	void jp(bool condition);
	void jp_u16();
	void jp_hl();
	void rlca();
	void rrca();
	void rra();
	void rla();
	void daa();
	void scf();
	void halt();
	void ret();
	void ret(bool condition);
	void reti();
	void rst_vector(uint8_t vector);

	//CB Opcodes
	void UnimplementedOpcode();
	void RunCBOpcode();
	void rlc(uint8_t& reg);
	void rlc_at_address();
};

