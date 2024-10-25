#include <variant>
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
	HalfCarry = ((((original_value + 1) & 0xF) - (1 & 0xF)) & 0x10) == 0x10;
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

void GameBoy::ld_mem_8bit()
{
	PC++;
	writeBus(getBus(PC), getHL());
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::ld_to_mem(uint8_t& reg_a, uint8_t& reg_b, uint8_t value)
{
	writeBus(value, (reg_a << 8) | reg_b);
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::ld_u8_to_mem()
{
	PC++;
	uint8_t value = getBus(PC);
	writeBus(value, getHL());
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::ld_sp_to_u16()
{
	uint16_t address = get_next_two_bytes(PC);
	writeBus(SP & 0xff, address);
	writeBus(SP >> 8, address + 1);
	PC += 3;
	add_m_cycles(3);
	add_t_cycles(12);
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

void GameBoy::ld_hl_sp_i8()
{
	PC++;
	setHL(SP);
	setHL(getHL() + (int)getBus(PC));
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::ld_hram_c_a()
{
	writeBus(A, 0xFF00 + C);
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::ld_a_hram_c()
{
	A = getBus(0xFF00 + C);
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

void GameBoy::add_a_address()
{
	uint16_t original_val = A;
	uint8_t value = getBus(getHL());
	A += value;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
	NegativeFlag = 0;
	ZeroFlag = !(A);
	HalfCarry = (((original_val & 0xF) + (value & 0xF)) & 0x10) == 0x10;
	Carry = (original_val + value) >> 8 > 0;
}

void GameBoy::add_hl_16bit(uint8_t reg_a, uint8_t reg_b)
{
	uint16_t value = fuse_two_bytes(reg_a, reg_b);
	uint16_t hl = getHL();
	setHL(hl + value);
	NegativeFlag = 0;
	HalfCarry = (((hl & 0xFFF) + (value & 0xFFF)) & 0x1000) == 0x1000;
	Carry = ((uint32_t)hl + value) >> 16 > 0;
	PC++;
}

void GameBoy::add_sp_i8()
{
	uint16_t orig = SP;
	PC++;
	uint16_t value = getBus(PC);
	SP += value;
	ZeroFlag = 0;
	NegativeFlag = 0;
	HalfCarry = (((orig & 0xFFF) + (value & 0xFFF)) & 0x1000) == 0x1000;
	Carry = ((uint32_t)orig + value) >> 16 > 0;
	PC++;
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

void GameBoy::sub_a_address()
{
	uint16_t original_val = A;
	uint8_t value = getBus(getHL());
	A = A - value; // Use subtraction instead of adding negative value
	add_m_cycles(2);
	add_t_cycles(8);
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

void GameBoy::or_a_reg(uint8_t reg)
{
	A |= reg;
	ZeroFlag = !A;
	NegativeFlag = 0;
	Carry = 0;
	HalfCarry = 0;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
}

void GameBoy::or_a_u8()
{
	PC++;
	A |= getBus(PC);
	ZeroFlag = !A;
	NegativeFlag = 0;
	Carry = 0;
	HalfCarry = 0;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::or_a_hl()
{
	A |= getBus(getHL());
	ZeroFlag = !A;
	NegativeFlag = 0;
	Carry = 0;
	HalfCarry = 1;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::xor_a_reg(uint8_t reg)
{
	A ^= reg;
	ZeroFlag = !A;
	NegativeFlag = 0;
	Carry = 0;
	HalfCarry = 0;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
}

void GameBoy::xor_a_u8()
{
	PC++;
	A ^= getBus(PC);
	ZeroFlag = !A;
	NegativeFlag = 0;
	Carry = 0;
	HalfCarry = 0;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
}

void GameBoy::xor_a_hl()
{
	A ^= getBus(getHL());
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

void GameBoy::sbc_a_reg(uint8_t reg)
{
	uint16_t original_val = A;
	A -= (reg + Carry);
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
	NegativeFlag = 1;
	ZeroFlag = !(A);
	HalfCarry = (((original_val & 0xF) - (reg & 0xF) - (Carry & 0xF)) & 0x10) == 0x10;
	Carry = (original_val - reg - Carry) >> 8 > 0;
}

void GameBoy::sbc_a_u8()
{
	uint16_t original_val = A;
	PC++;
	uint8_t value = getBus(PC) + Carry;
	A -= value;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
	NegativeFlag = 1;
	ZeroFlag = !(A);
	HalfCarry = (((original_val & 0xF) - (value & 0xF)) & 0x10) == 0x10;
	Carry = (original_val - value) >> 8 > 0;
}

void GameBoy::sbc_a_hl()
{
	uint16_t original_val = A;
	uint8_t value = getBus(getHL()) + Carry;
	A -= value;
	add_m_cycles(2);
	add_t_cycles(8);
	PC++;
	NegativeFlag = 1;
	ZeroFlag = !(A);
	HalfCarry = (((original_val & 0xF) - (value & 0xF)) & 0x10) == 0x10;
	Carry = (original_val - value) >> 8 > 0;
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

void GameBoy::call(bool condition)
{
	if (condition)
	{
		PUSH_STACK_16BIT(PC + 3);
		PC = get_next_two_bytes(PC);
		add_m_cycles(6);
		add_t_cycles(24);
	}
	else
	{
		add_m_cycles(3);
		add_t_cycles(12);
		PC += 3;
	}
}

void GameBoy::call_u16()
{
	PUSH_STACK_16BIT(PC + 3);
	PC = get_next_two_bytes(PC);
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

void GameBoy::rlca()
{
	// extract msb
	uint8_t msb = (A & 0b10000000) >> 7;
	// rotate a and append msb
	A = (A << 1) | msb;
	// set carry
	Carry = msb;
	// Set other
	ZeroFlag = 0;
	NegativeFlag = 0;
	HalfCarry = 0;
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::rrca()
{
	// extract lsb
	uint8_t lsb = A & 0x1;
	// rotate a and append msb
	A = (A >> 1) | (lsb << 7);
	// set carry
	Carry = lsb;
	// Set other
	ZeroFlag = 0;
	NegativeFlag = 0;
	HalfCarry = 0;
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::rra()
{
	// get lsb
	uint8_t lsb = A & 0x1;

	// rotate a and add carry
	A = (A >> 1) | (Carry << 7);

	// set carry
	Carry = lsb;
	// Set other
	ZeroFlag = 0;
	NegativeFlag = 0;
	HalfCarry = 0;
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::rla()
{
	// get msb
	uint8_t msb = A >> 7;

	// rotate a and add carry
	A = (A << 1) | (Carry);

	// set carry
	Carry = msb;
	// Set other
	ZeroFlag = 0;
	NegativeFlag = 0;
	HalfCarry = 0;
	add_m_cycles(3);
	add_t_cycles(12);
	PC++;
}

void GameBoy::daa()
{
	/* From: https://forums.nesdev.org/viewtopic.php?t=15944 */
	// note: assumes a is a uint8_t and wraps from 0xff to 0
	if (!NegativeFlag) {  // after an addition, adjust if (half-)carry occurred or if result is out of bounds
		if (Carry || A > 0x99) { A += 0x60; Carry = 1; }
		if (HalfCarry || (A & 0x0f) > 0x09) { A += 0x6; }
	}
	else {  // after a subtraction, only adjust if (half-)carry occurred
		if (Carry) { A -= 0x60; }
		if (HalfCarry) { A -= 0x6; }
	}
	// these flags are always updated
	ZeroFlag = (A == 0); // the usual z flag
	HalfCarry = 0; // h flag is always cleared
	// Set other
	HalfCarry = 0;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
}

void GameBoy::scf()
{
	NegativeFlag = 0;
	HalfCarry = 0;
	Carry = 1;
	add_m_cycles(1);
	add_t_cycles(4);
	PC++;
}

void GameBoy::halt()
{
	halted = true;
	PC++;
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
