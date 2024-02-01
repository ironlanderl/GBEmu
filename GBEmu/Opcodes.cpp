#include "GameBoy.h"

void GameBoy::opcode_nop()
{
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
}

void GameBoy::dec_8bit_register(uint8_t &reg)
{
	HalfCarry = (((reg & 0xF) + (-1 & 0xF)) & 0x10) == 0x10;
	reg--;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
	NegativeFlag = 1;
	ZeroFlag = !(reg);
}

void GameBoy::dec_16bit_register(uint8_t& reg_a, uint8_t& reg_b)
{
	uint16_t fused = fuse_two_bytes(reg_a, reg_b);
	fused--;
	reg_a = fused & 0xFF00 >> 8;
	reg_b = fused & 0x00FF;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::dec_16bit_register(uint16_t& reg)
{
	reg--;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;

}

void GameBoy::inc_8bit_register(uint8_t& reg)
{
	HalfCarry = (((reg & 0xF) + (1 & 0xF)) & 0x10) == 0x10;
	reg++;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
	NegativeFlag = 0;
	ZeroFlag = !(reg);
}

void GameBoy::inc_16bit_register(uint8_t& reg_a, uint8_t& reg_b)
{
	uint16_t fused = fuse_two_bytes(reg_a, reg_b);
	fused++;
	reg_a = fused & 0xFF00 >> 8;
	reg_b = fused & 0x00FF;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::inc_16bit_register(uint16_t& reg)
{
	reg++;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::dec_at_address()
{
	uint8_t original_value = getBus(getHL());
	HalfCarry = (((original_value & 0xF) + (-1 & 0xF)) & 0x10) == 0x10;
	original_value--;
	writeBus(original_value, getHL());
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
	NegativeFlag = 1;
	ZeroFlag = !(original_value);
}

void GameBoy::inc_at_address()
{
	uint8_t original_value = getBus(getHL());
	HalfCarry = (((original_value & 0xF) + (1 & 0xF)) & 0x10) == 0x10;
	original_value++;
	writeBus(original_value, getHL());
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
	NegativeFlag = 0;
	ZeroFlag = !(original_value);
}

void GameBoy::ld_8bit_register(uint8_t& dest, uint8_t source)
{
	dest = source;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
}

void GameBoy::ld_8bit_value(uint8_t& dest, uint8_t value)
{
	dest = value;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::ld_16bit_value(uint8_t& dest_a, uint8_t& dest_b, uint16_t value)
{
	dest_a = value & 0xFF00 >> 8;
	dest_b = value & 0x00FF;
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::ld_16bit_value(uint16_t& dest, uint16_t value)
{
	dest = value;
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::rst_vector(uint8_t vector)
{
	PUSH_STACK_16BIT(PC);
	add_m_cycles(4);
	add_t_cycles(16);
	PC = vector;
}
