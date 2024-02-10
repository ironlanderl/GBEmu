#include "GameBoy.h"

#include <variant>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

void GameBoy::writeBusUnrestricted(uint8_t value, uint16_t address)
{
	// Map address to appropriate memory array
	if (isInsideInterval(address, 0x0000, 0x7FFF))
	{
		// Write to ROM or handle bank switching
		// Depending on your implementation
		ROM[address] = value;
	}
	else if (isInsideInterval(address, 0x8000, 0x9FFF))
	{
		VRAM[address - 0x8000] = value;
	}
	else if (isInsideInterval(address, 0xA000, 0xBFFF))
	{
		WRAM0[address - 0xA000] = value;
	}
	else if (isInsideInterval(address, 0xC000, 0xDFFF))
	{
		WRAM1[address - 0xC000] = value;
	}
	else if (isInsideInterval(address, 0xE000, 0xFDFF))
	{
		WRAM0[address - 0xE000] = value; // Echo RAM
	}
	else if (isInsideInterval(address, 0xFE00, 0xFE9F))
	{
		OAM[address - 0xFE00] = value;
	}
	else if (isInsideInterval(address, 0xFEA0, 0xFEFF))
	{
		PROHIBITED[address - 0xFEA0] = value;
	}
	else if (isInsideInterval(address, 0xFF00, 0xFF7F))
	{
		HRAM[address - 0xFF00] = value;
	}
	else if (isInsideInterval(address, 0xFF80, 0xFFFE))
	{
		HRAM[address - 0xFF80] = value;
	}
	else if (address == 0xFFFF)
	{
		IE = value;
	}
	else
	{
		std::printf("Tried writing to invalid address: %04X\n", address);
		status = PAUSED;
	}
}

void GameBoy::writeBus(uint8_t value, uint16_t address)
{
	// TODO: ABSOLUTELY REPLACE
	writeBusUnrestricted(value, address);
	return;
	if (isInsideInterval(address, 0x0000, 0x7FFF))
	{
		ROM[address] = value; // Should probably disable at some point
	}
	// Only allow writing in WRAM.
	else if (isInsideInterval(address, 0xA000, 0xBFFF))
	{
		WRAM0[address - 0xA000] = value;
	}
	else if (isInsideInterval(address, 0xC000, 0xDFFF))
	{
		WRAM1[address - 0xC000] = value;
	}
	else if(isInsideInterval(address, 0xFF00, 0xFFFE))
	{
		HRAM[address - 0xFF00] = value;
	}
	else
	{
		std::printf("Tried writing to invalid address: %04X\n", address);
	}
}

uint8_t GameBoy::getBus(uint16_t address)
{
	// Map address to memory appropriate array
	if (isInsideInterval(address, 0x0000, 0x3FFF))
	{
		return ROM[address];
	}
	else if (isInsideInterval(address, 0x4000, 0x7FFF))
	{
		return handleMapperReads(address);
	}
	else if (isInsideInterval(address, 0x8000, 0x9FFF))
	{
		return VRAM[address - 0x8000];
	}
	else if (isInsideInterval(address, 0xA000, 0xBFFF))
	{
		return WRAM0[address - 0xA000];
	}
	else if (isInsideInterval(address, 0xC000, 0xDFFF))
	{
		return WRAM1[address - 0xC000];
	}
	else if (isInsideInterval(address, 0xE000, 0xFDFF))
	{
		return WRAM0[address - 0xE000]; // ECHO RAM
	}
	else if (isInsideInterval(address, 0xFE00, 0xFE9F))
	{
		return OAM[address - 0xFE00];
	}
	else if (isInsideInterval(address, 0xFEA0, 0xFEFF))
	{
		return PROHIBITED[address - 0xFEA0];
	}
	else if (isInsideInterval(address, 0xFF00, 0xFF7F))
	{
		// TODO - HANDLE INPUT
		return HRAM[address - 0xFF00];
	}
	else if (isInsideInterval(address, 0xFF80, 0xFFFE))
	{
		return HRAM[address - 0xFF80];
	}
	else if (address == 0xFFFF)
	{
		return IE;
	}
	else
	{
		std::printf("Tried accessing invalid address: %04X", address);
		status = PAUSED;
	}
}

void GameBoy::loadRom(char cart[], std::streamsize fileSize)
{
	memcpy(&ROM, cart, fileSize);
	// Set register to default
	PC = 0x0100;
	A = 0x01;
	B = 0x00;
	C = 0x13;
	D = 0x00;
	E = 0xD8;
	ZeroFlag = 1;
	H = 0x01;
	L = 0x4D;
	SP = 0xFFFE;
	IME = false;
}

void GameBoy::PUSH_STACK_8BIT(uint8_t value)
{
	SP--;
	writeBus(value, SP); // TODO: USE RESTRICTED
}

void GameBoy::PUSH_STACK_16BIT(uint16_t value)
{
	PUSH_STACK_8BIT(value >> 8);
	PUSH_STACK_8BIT(value & 0x00FF);
}

uint8_t GameBoy::POP_STACK_8BIT()
{
	uint8_t value = getBus(SP);
	SP++;
	return value;
}

uint16_t GameBoy::POP_STACK_16BIT()
{
	uint16_t value;
	value = POP_STACK_8BIT();
	value |= POP_STACK_8BIT() << 8;
	return value;
}

void GameBoy::handleMapperWrites(uint8_t value, uint16_t address)
{
	switch (getBus(0x0147))
	{
	case 0x00: // NO MAPPER
		return;
	case 0x01: // MBC 1
		if (isInsideInterval(address, 0x0000, 0x1FFF)) // RAM ENABLE - DISABLE
		{
			mbc1_settings.RAM_ENABLED = value & 0x000F == 0xA;
		}
		if (isInsideInterval(address, 0x2000, 0x3FFF)) // ROM BANK SELECT
		{
			mbc1_settings.ROM_BANK = value & 0b11111;
		}
		if (isInsideInterval(address, 0x4000, 0x5FFF)) // RAM BANK SELECT
		{
			mbc1_settings.RAM_BANK = value & 0x11;
		}
		if (isInsideInterval(address, 0x6000, 0x7FFF)) // BANK MODE SELECTOR
		{
			fmt::println("Cart Tried to switch BANKING MODE");
		}
		break;
	default:
		fmt::println("Unsupported mapper");
		break;
	}
}

uint8_t GameBoy::handleMapperReads(uint16_t address)
{
	switch (getBus(0x0147))
	{
	case 0x00: // NO MAPPER
		return ROM[address];
	case 0x01: // MBC 1
		// TODO - IMPLEMENT MODE SWITCHING READS
		if (isInsideInterval(address, 0x4000, 0x7FFF)) {
			int BANK_TO_READ_FROM = -1;
			if (mbc1_settings.RAM_BANK == 0x00)
				BANK_TO_READ_FROM = 0x4000;
			else
				BANK_TO_READ_FROM = 0x4000 * mbc1_settings.ROM_BANK;
			return ROM[address + BANK_TO_READ_FROM];
		}
	default:
		fmt::println("Unsupported mapper");
		break;
	}
}

void GameBoy::add_m_cycles(uint8_t cycles_to_add)
{
	m_cycles += cycles_to_add;
}

void GameBoy::add_t_cycles(uint8_t cycles_to_add)
{
	t_cycles += cycles_to_add;
}

void GameBoy::advanceStep()
{
	switch (getBus(PC))
	{
	case 0x00: // NOP
		opcode_nop();
		break;
		// Area INC 16 bit
	case 0x03:
		inc_16bit_register(B, C);
		break;
	case 0x13:
		inc_16bit_register(D, E);
		break;
	case 0x23:
		inc_16bit_register(H, L);
		break;
	case 0x33:
		inc_16bit_register(SP);
		break;
		// Area DEC 16 bit
	case 0x0B:
		dec_16bit_register(B, C);
		break;
	case 0x1B:
		dec_16bit_register(D, E);
		break;
	case 0x2B:
		dec_16bit_register(H, L);
		break;
	case 0x3B:
		dec_16bit_register(SP);
		break;
		// Area DEC 8bit
	case 0x05:
		dec_8bit_register(B);
		break;
	case 0x15:
		dec_8bit_register(D);
		break;
	case 0x25:
		dec_8bit_register(H);
		break;
	case 0x35:
		dec_at_address();
		break;
	case 0x0D:
		dec_8bit_register(C);
		break;
	case 0x1D:
		dec_8bit_register(E);
		break;
	case 0x2D:
		dec_8bit_register(L);
		break;
	case 0x3D:
		dec_8bit_register(A);
		break;
		// Area INC 8 bit
	case 0x04:
		inc_8bit_register(B);
		break;
	case 0x14:
		inc_8bit_register(D);
		break;
	case 0x24:
		inc_8bit_register(H);
		break;
	case 0x34:
		inc_at_address();
		break;
	case 0x0C:
		inc_8bit_register(C);
		break;
	case 0x1C:
		inc_8bit_register(E);
		break;
	case 0x2C:
		inc_8bit_register(L);
		break;
	case 0x3C:
		inc_8bit_register(A);
		break;
		// Area LD Reg,Reg
	case 0x40:
		ld_8bit_register(B, B);
		break;
	case 0x50:
		ld_8bit_register(D, B);
		break;
	case 0x60:
		ld_8bit_register(H, B);
		break;
	case 0x41:
		ld_8bit_register(B, C);
		break;
	case 0x51:
		ld_8bit_register(D, C);
		break;
	case 0x61:
		ld_8bit_register(H, C);
		break;
	case 0x42:
		ld_8bit_register(B, D);
		break;
	case 0x52:
		ld_8bit_register(D, D);
		break;
	case 0x62:
		ld_8bit_register(H, D);
		break;
	case 0x43:
		ld_8bit_register(B, E);
		break;
	case 0x53:
		ld_8bit_register(D, E);
		break;
	case 0x63:
		ld_8bit_register(H, E);
		break;
	case 0x44:
		ld_8bit_register(B, H);
		break;
	case 0x54:
		ld_8bit_register(D, H);
		break;
	case 0x64:
		ld_8bit_register(H, H);
		break;
	case 0x45:
		ld_8bit_register(B, L);
		break;
	case 0x55:
		ld_8bit_register(D, L);
		break;
	case 0x65:
		ld_8bit_register(H, L);
		break;
	case 0x47:
		ld_8bit_register(B, A);
		break;
	case 0x57:
		ld_8bit_register(D, A);
		break;
	case 0x67:
		ld_8bit_register(H, A);
		break;
	case 0x48:
		ld_8bit_register(C, B);
		break;
	case 0x58:
		ld_8bit_register(E, B);
		break;
	case 0x68:
		ld_8bit_register(L, B);
		break;
	case 0x78:
		ld_8bit_register(A, B);
		break;
	case 0x49:
		ld_8bit_register(C, C);
		break;
	case 0x59:
		ld_8bit_register(E, C);
		break;
	case 0x69:
		ld_8bit_register(L, C);
		break;
	case 0x79:
		ld_8bit_register(A, C);
		break;
	case 0x4A:
		ld_8bit_register(C, D);
		break;
	case 0x5A:
		ld_8bit_register(E, D);
		break;
	case 0x6A:
		ld_8bit_register(L, D);
		break;
	case 0x7A:
		ld_8bit_register(A, D);
		break;
	case 0x4B:
		ld_8bit_register(C, E);
		break;
	case 0x5B:
		ld_8bit_register(E, E);
		break;
	case 0x6B:
		ld_8bit_register(L, E);
		break;
	case 0x7B:
		ld_8bit_register(A, E);
		break;
	case 0x4C:
		ld_8bit_register(C, H);
		break;
	case 0x5C:
		ld_8bit_register(E, H);
		break;
	case 0x6C:
		ld_8bit_register(L, H);
		break;
	case 0x7C:
		ld_8bit_register(A, H);
		break;
	case 0x4D:
		ld_8bit_register(C, L);
		break;
	case 0x5D:
		ld_8bit_register(E, L);
		break;
	case 0x6D:
		ld_8bit_register(L, L);
		break;
	case 0x7D:
		ld_8bit_register(A, L);
		break;
	case 0x4F:
		ld_8bit_register(C, A);
		break;
	case 0x5F:
		ld_8bit_register(E, A);
		break;
	case 0x6F:
		ld_8bit_register(L, A);
		break;
	case 0x7F:
		ld_8bit_register(A, A);
		break;
		// Area LD reg, 8bit val
	case 0x06:
		ld_8bit_value(B, get_next_byte(PC));
		break;
	case 0x16:
		ld_8bit_value(D, get_next_byte(PC));
		break;
	case 0x26:
		ld_8bit_value(H, get_next_byte(PC));
		break;
	case 0x0E:
		ld_8bit_value(C, get_next_byte(PC));
		break;
	case 0x1E:
		ld_8bit_value(E, get_next_byte(PC));
		break;
	case 0x2E:
		ld_8bit_value(L, get_next_byte(PC));
		break;
	case 0x3E:
		ld_8bit_value(A, get_next_byte(PC));
		break;
		// Area LD reg, 16bit val
	case 0x01:
		ld_16bit_value(B, C, get_next_two_bytes(PC));
		break;
	case 0x11:
		ld_16bit_value(D, E, get_next_two_bytes(PC));
		break;
	case 0x21:
		ld_16bit_value(H, L, get_next_two_bytes(PC));
		break;
	case 0x31:
		ld_16bit_value(SP, get_next_two_bytes(PC));
		break;
	case 0xEA:
		ld_a_to_address();
		break;
	case 0xE0:
		ld_a_to_wram_offset();
		break;
	case 0xF0:
		ld_wram_offset_to_a();
		break;
	// Vector Jump
	case 0xC7:
		rst_vector(0x00);
		break;
	case 0xD7:
		rst_vector(0x10);
		break;
	case 0xE7:
		rst_vector(0x20);
		break;
	case 0xF7:
		rst_vector(0x30);
		break;
	case 0xCF:
		rst_vector(0x08);
		break;
	case 0xDF:
		rst_vector(0x18);
		break;
	case 0xEF:
		rst_vector(0x28);
		break;
	case 0xFF:
		rst_vector(0x38);
		break;
	// Area LD reg, (mem)
	case 0x4E:
		ld_8bit_value_from_ram(C, (H << 8) | L);
		break;
	case 0x5E:
		ld_8bit_value_from_ram(E, (H << 8) | L);
		break;
	case 0x6E:
		ld_8bit_value_from_ram(L, (H << 8) | L);
		break;
	case 0x7E:
		ld_8bit_value_from_ram(A, (H << 8) | L);
		break;
	// Area ADD, reg
	case 0x80:
		add_a_reg(B);
		break;
	case 0x81:
		add_a_reg(C);
		break;
	case 0x82:
		add_a_reg(D);
		break;
	case 0x83:
		add_a_reg(E);
		break;
	case 0x84:
		add_a_reg(H);
		break;
	case 0x85:
		add_a_reg(L);
		break;
	case 0x87:
		add_a_reg(A);
		break;
		// Area DEC, reg
	case 0x90:
		sub_a_reg(B);
		break;
	case 0x91:
		sub_a_reg(C);
		break;
	case 0x92:
		sub_a_reg(D);
		break;
	case 0x93:
		sub_a_reg(E);
		break;
	case 0x94:
		sub_a_reg(H);
		break;
	case 0x95:
		sub_a_reg(L);
		break;
	case 0x97:
		sub_a_reg(A);
		break;
	// Area PUSH / POP
	case 0xC1:
		pop_stack(B, C);
		break;
	case 0xD1:
		pop_stack(D,E);
		break;
	case 0xE1:
		pop_stack(H,L);
		break;
	case 0xC5:
		push_stack(B,C);
		break;
	case 0xD5:
		push_stack(D, E);
		break;
	case 0xE5:
		push_stack(H, L);
		break;
	case 0xF1: // POP AF
		uint8_t value_f;
		value_f = getBus(SP);
		ZeroFlag = (value_f & 128) >> 7;
		NegativeFlag = (value_f & 64) >> 6;
		HalfCarry = (value_f & 32) >> 5;
		Carry = (value_f & 0x16) >> 4;
		SP++;
		A = getBus(SP);
		SP++;
		add_m_cycles(3);
		add_t_cycles(12);
		PC++;
		break;
	case 0xF5: // PUSH AF
		SP--;
		writeBus(A, SP);
		uint8_t value_f_write;
		value_f_write = 0;
		value_f_write = value_f_write | (ZeroFlag << 7);
		value_f_write = value_f_write | (NegativeFlag << 6);
		value_f_write = value_f_write | (HalfCarry << 5);
		value_f_write = value_f_write | (Carry << 4);
		SP--;
		writeBus(value_f_write, SP);
		add_m_cycles(3);
		add_t_cycles(12);
		PC++;
		break;
	// Area AND
	case 0xA0:
		and_a_reg(B);
		break;
	case 0xA1:
		and_a_reg(C);
		break;
	case 0xA2:
		and_a_reg(D);
		break;
	case 0xA3:
		and_a_reg(E);
		break;
	case 0xA4:
		and_a_reg(H);
		break;
	case 0xA5:
		and_a_reg(L);
		break;
	case 0xA6:
		and_a_hl();
		break;
	case 0xA7:
		and_a_reg(A);
		break;
	case 0xE6:
		and_a_u8();
		break;
	// Area CP
	case 0xB8:
		cp_a_reg(B);
		break;
	case 0xB9:
		cp_a_reg(C);
		break;
	case 0xBA:
		cp_a_reg(D);
		break;
	case 0xBB:
		cp_a_reg(E);
		break;
	case 0xBC:
		cp_a_reg(H);
		break;
	case 0xBD:
		cp_a_reg(L);
		break;
	case 0xBE:
		cp_a_hl();
		break;
	case 0xBF:
		cp_a_reg(A);
		break;
	case 0xFE:
		cp_a_u8();
		break;
	// Area ADC
	case 0x88:
		adc_a_reg(B);
		break;
	case 0x89:
		adc_a_reg(C);
		break;
	case 0x8A:
		adc_a_reg(D);
		break;
	case 0x8B:
		adc_a_reg(E);
		break;
	case 0x8C:
		adc_a_reg(H);
		break;
	case 0x8D:
		adc_a_reg(L);
		break;
	case 0x8E:
		adc_a_hl();
		break;
	case 0x8F:
		adc_a_reg(A);
		break;
	case 0xCE:
		adc_a_u8();
		break;
	// Area JR/JP
	case 0x20: // JR NZ
		jr(ZeroFlag);
		break;
	case 0x30: // JR NZ
		jr(Carry);
		break;
	case 0x28: // JR NZ
		jr(!ZeroFlag);
		break;
	case 0x38: // JR NZ
		jr(!Carry);
		break;
	case 0x18: // JR i8
		jr_i8();
		break;
	case 0xC2: // JP NZ
		jp(ZeroFlag);
		break;
	case 0xD2: // JP NC
		jp(Carry);
		break;
	case 0xCA: // JP Z
		jp(!ZeroFlag);
		break;
	case 0xDA: // JP C
		jp(!Carry);
		break;
	case 0xC3: // JP u16
		jp_u16();
		break;
	case 0xE9: // JP HL
		jp_hl();
		break;
	// Area ret
	case 0xC0:
		ret(ZeroFlag);
		break;
	case 0xD0:
		ret(Carry);
		break;
	case 0xC8:
		ret(!ZeroFlag);
		break;
	case 0xD8:
		ret(!Carry);
		break;
	case 0xC9:
		ret();
		break;
	case 0xD9:
		reti();
		break;
		// Continuo
	case 0x10: // STOP
		add_m_cycles(1);
		add_t_cycles(4);
		PC += 2;
		break;
	case 0x32: // LD [HL-], A
		writeBusUnrestricted(A, getHL()); // TODO: Put IT Back To Restricted / Accurate
		setHL(getHL() - 1);
		add_m_cycles(2);
		add_t_cycles(8);
		PC += 1;
		break;
	case 0xAF: // XOR A?
		A ^= A;
		ZeroFlag = !A;
		NegativeFlag = 0;
		Carry = 0;
		HalfCarry = 0;
		add_m_cycles(1);
		add_t_cycles(4);
		PC++;
		break;
		// MISC / UNGENERAL ENOUGH
	case 0x2F:
		A = ~A;
		NegativeFlag = 1;
		HalfCarry = 1;
		add_m_cycles(1);
		add_t_cycles(4);
		PC++;
		break;
	case 0xCD:
		call_u16();
		break;
	case 0xF3: // Disable Interrupts
		IME = false;
		add_m_cycles(1);
		add_t_cycles(4);
		PC++;
		break;
	case 0xFB:
		IME = true;
		add_m_cycles(1);
		add_t_cycles(4);
		PC++;
		break;
	case 0xFA: // LD A, (u16)
		A = getBus(get_next_two_bytes(PC));
		PC+=3;
		add_m_cycles(4);
		add_t_cycles(16);
		break;
	case 0xC6: // ADD A, u8
		add_a_u8();
		break;
	case 0xD6: // SUB A, u8
		sub_a_u8();
		break;
	case 0xF9: // LD SP, HL
		SP = H << 8 | L;
		add_m_cycles(2);
		add_t_cycles(8);
		PC++;
		break;
	default:
		std::printf("Invalid opcode: %02X \n", getBus(PC));
		status = PAUSED;
		//exit(-1);
		break;
	}
}

bool GameBoy::isInsideInterval(uint16_t value, uint16_t bottom_bound, uint16_t upper_bound)
{
	return value <= upper_bound && value >= bottom_bound;
}

uint16_t GameBoy::get_next_two_bytes(uint16_t address)
{
	return (getBus(address + 1) | (getBus(address + 2) << 8));
}

uint8_t GameBoy::get_next_byte(uint16_t address)
{
	return getBus(address + 1);
}

uint16_t GameBoy::fuse_two_bytes(uint8_t a, uint8_t b)
{
	return (a << 8) | b;
}

uint16_t GameBoy::getHL()
{
	return ((H << 8) | L);
}

void GameBoy::setHL(uint16_t value)
{
	H = value >> 8;
	L = value & 0xFF;
}
