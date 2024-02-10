#include <corecrt_io.h>
#include <variant>
#include <windows.h>

#include "GameBoy.h"

void GameBoy::opcode_nop()
{
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
}

void GameBoy::dec_8bit_register(uint8_t& reg)
{
	uint8_t reg_copy = reg;
	reg--;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
	NegativeFlag = 1;
	ZeroFlag = !(reg);
	HalfCarry = (((reg_copy & 0xF) - (1 & 0xF)) & 0x10) == 0x10;
}

void GameBoy::dec_16bit_register(uint8_t& reg_a, uint8_t& reg_b)
{
	uint16_t fused = fuse_two_bytes(reg_a, reg_b);
	fused--;
	reg_a = (fused & 0xFF00) >> 8;
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
	uint8_t reg_copy = reg;
	reg++;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
	NegativeFlag = 0;
	ZeroFlag = !(reg);
	HalfCarry = (((reg_copy & 0xF) + (1 & 0xF)) & 0x10) == 0x10;
}

void GameBoy::inc_16bit_register(uint8_t& reg_a, uint8_t& reg_b)
{
	uint16_t fused = fuse_two_bytes(reg_a, reg_b);
	fused++;
	reg_a = (fused & 0xFF00) >> 8;
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
	original_value--;
	writeBus(original_value, getHL());
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
	NegativeFlag = 1;
	ZeroFlag = !(original_value);
	HalfCarry = ((((original_value - 1) & 0xF) - (1 & 0xF)) & 0x10) == 0x10;
}

void GameBoy::inc_at_address()
{
	uint8_t original_value = getBus(getHL());
	original_value++;
	writeBus(original_value, getHL());
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
	NegativeFlag = 0;
	ZeroFlag = !(original_value);
	HalfCarry = ((((original_value - 1) & 0xF) + (1 & 0xF)) & 0x10) == 0x10;
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
	PC += 2;
}

void GameBoy::ld_16bit_value(uint8_t& dest_a, uint8_t& dest_b, uint16_t value)
{
	dest_a = (value & 0xFF00) >> 8;
	dest_b = value & 0x00FF;
	add_m_cycles(3);
	add_t_cycles(12);
	PC += 3;
}

void GameBoy::ld_16bit_value(uint16_t& dest, uint16_t value)
{
	dest = value;
	add_m_cycles(3);
	add_t_cycles(12);
	PC += 3;
}

void GameBoy::ld_a_to_address()
{
	writeBus(A, getBus(PC + 1) | getBus(PC + 2) << 8);
	add_m_cycles(4);
	add_t_cycles(16);
	PC += 3;
}

void GameBoy::ld_a_to_wram_offset()
{
	writeBusUnrestricted(A, getBus(PC + 1) + 0xFF00); // TODO: USE RESTRICTED
	add_m_cycles(3);
	add_t_cycles(12);
	PC += 2;
}

void GameBoy::ld_wram_offset_to_a()
{
	A = getBus(getBus(PC + 1) + 0xFF00);
	add_m_cycles(3);
	add_t_cycles(12);
	PC += 2;
}

void GameBoy::ld_8bit_value_from_ram(uint8_t& dest, uint16_t source)
{
	dest = getBus(source);
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::add_a_u8()
{
	uint16_t original_val = A;
	PC++;
	uint8_t value = getBus(PC);
	A += value;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
	NegativeFlag = 0;
	ZeroFlag = !(A);
	HalfCarry = (((original_val & 0xF) + (value & 0xF)) & 0x10) == 0x10;
	Carry = (original_val + value) >> 8 > 0;
}

void GameBoy::add_a_reg(uint8_t reg)
{
	uint16_t original_val = A;
	A += reg;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
	NegativeFlag = 0;
	ZeroFlag = !(A);
	HalfCarry = (((original_val & 0xF) + (reg & 0xF)) & 0x10) == 0x10;
	Carry = (original_val + reg) >> 8 > 0;
}

void GameBoy::sub_a_u8() {
	uint16_t original_val = A;
	PC++;
	uint8_t value = getBus(PC);
	A = A - value; // Use subtraction instead of adding negative value
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
	NegativeFlag = 1;
	ZeroFlag = !(A);
	HalfCarry = (original_val & 0xF) < (value & 0xF);
	Carry = (value) > original_val;
}

void GameBoy::sub_a_reg(uint8_t reg) {
	uint16_t original_val = A;
	uint8_t value = reg;
	A = A - value; // Use subtraction instead of adding negative value
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
	NegativeFlag = 1;
	ZeroFlag = !(A);
	HalfCarry = (original_val & 0xF) < (value & 0xF);
	Carry = (value) > original_val;
}

void GameBoy::and_a_reg(uint8_t reg)
{
	A = A & reg;
	ZeroFlag = !A;
	NegativeFlag = 0;
	Carry = 0;
	HalfCarry = 1;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
}

void GameBoy::and_a_u8()
{
	PC++;
	A = A & getBus(PC);
	ZeroFlag = !A;
	NegativeFlag = 0;
	Carry = 0;
	HalfCarry = 1;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::and_a_hl()
{
	A = A & getBus(getHL());
	ZeroFlag = !A;
	NegativeFlag = 0;
	Carry = 0;
	HalfCarry = 1;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::cp_a_reg(uint8_t reg)
{
	uint8_t res = A - reg;
	Carry = reg > A;
	NegativeFlag = 1;
	HalfCarry = (A & 0xF) < (reg & 0xF);
	ZeroFlag = !res;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
}

void GameBoy::cp_a_u8()
{
	PC++;
	uint8_t reg = getBus(PC);
	uint8_t res = A - reg;
	NegativeFlag = 1;
	Carry = reg > A;
	HalfCarry = (A & 0xF) < (reg & 0xF);
	ZeroFlag = !res;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::cp_a_hl()
{
	uint8_t reg = getBus(getHL());
	uint8_t res = A - reg;
	Carry = reg > A;
	NegativeFlag = 1;
	HalfCarry = (A & 0xF) < (reg & 0xF);
	ZeroFlag = !res;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::adc_a_reg(uint8_t reg)
{
	uint16_t original_val = A;
	A += reg + Carry;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
	NegativeFlag = 0;
	ZeroFlag = !(A);
	HalfCarry = (((original_val & 0xF) + (reg & 0xF) + (Carry & 0xF)) & 0x10) == 0x10;
	Carry = (original_val + reg + Carry) >> 8 > 0;
}

void GameBoy::adc_a_u8()
{
	uint16_t original_val = A;
	PC++;
	uint8_t value = getBus(PC) + Carry;
	A += value;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
	NegativeFlag = 0;
	ZeroFlag = !(A);
	HalfCarry = (((original_val & 0xF) + (value & 0xF)) & 0x10) == 0x10;
	Carry = (original_val + value) >> 8 > 0;
}

void GameBoy::adc_a_hl()
{
	uint16_t original_val = A;
	uint8_t value = getBus(getHL()) + Carry;
	A += value;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
	NegativeFlag = 0;
	ZeroFlag = !(A);
	HalfCarry = (((original_val & 0xF) + (value & 0xF)) & 0x10) == 0x10;
	Carry = (original_val + value) >> 8 > 0;
}

void GameBoy::pop_stack(uint8_t& reg_a, uint8_t& reg_b)
{
	reg_b = getBus(SP);
	SP++;
	reg_a = getBus(SP);
	SP++;
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::push_stack(uint8_t& reg_a, uint8_t& reg_b)
{
	SP--;
	writeBus(reg_a, SP);
	SP--;
	writeBus(reg_b, SP);
	
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::call_u16()
{
	PC++;
	PUSH_STACK_16BIT(PC + 2);
	PC = getBus(PC) | getBus(PC + 1) << 8;
	add_m_cycles(6);
	add_t_cycles(24);
}

void GameBoy::jr(bool condition)
{
	if (condition == true)
	{
		add_m_cycles(2);
		add_t_cycles(8);
		PC += 2;
	}
	else
	{
		add_m_cycles(3);
		add_t_cycles(12);
		PC++;
		PC += static_cast<int8_t> (getBus(PC) + 1);
	}
}

void GameBoy::jr_i8()
{
	PC++;
	PC += static_cast<int8_t> (getBus(PC) + 1);
	add_m_cycles(3);
	add_t_cycles(12);
}

void GameBoy::jp(bool condition)
{
	if (condition == true)
	{
		add_m_cycles(2);
		add_t_cycles(8);
		PC += 3;
	}
	else
	{
		add_m_cycles(3);
		add_t_cycles(12);
		PC++;
		PC = getBus(PC) | getBus(PC + 1) << 8;
	}
}

void GameBoy::jp_u16()
{
	add_m_cycles(3);
	add_t_cycles(12);
	PC = getBus(PC + 1) | getBus(PC + 2) << 8;
}

void GameBoy::jp_hl()
{
	add_m_cycles(3);
	add_t_cycles(12);
	PC = getHL();
}

void GameBoy::ret()
{
	add_m_cycles(3);
	add_t_cycles(12);
	PC = POP_STACK_16BIT();
}

void GameBoy::ret(bool condition)
{
	if (condition == true)
	{
		add_m_cycles(2);
		add_t_cycles(8);
		PC++;
	}
	else
	{
		add_m_cycles(3);
		add_t_cycles(12);
		PC = POP_STACK_16BIT();
	}
}

void GameBoy::reti()
{
	ret();
	IME = true;
}

void GameBoy::rst_vector(uint8_t vector)
{
	PC++;
	PUSH_STACK_16BIT(PC);
	add_m_cycles(4);
	add_t_cycles(16);
	PC = vector;
}
