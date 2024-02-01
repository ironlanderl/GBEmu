#include "GameBoy.h"

#include <variant>
#include <fmt/core.h>

void GameBoy::writeBus(uint8_t value, uint16_t address)
{
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
		return VRAM[address - 0x8000]; // TODO
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
		return WRAM1[address - 0x2000]; // ECHO RAM TODO
	}
	else if (isInsideInterval(address, 0xFE00, 0xFE9F))
	{
		return OAM[address - 0xFE00];
	}
	else if (isInsideInterval(address, 0xFF00, 0xFF7F))
	{
		// TODO - HANDLE INPUT
		return HRAM[address - 0xFF80];
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
		throw std::runtime_error("Invalid address");
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
	writeBus(value, SP);
	SP--;
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
	value += POP_STACK_8BIT() << 8;
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
		// Continuo
	case 0x10: // STOP
		add_m_cycles(1);
		add_t_cycles(4);
		PC += 2;
		break;
	case 0x20:
		if (ZeroFlag == true)
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
			PC += static_cast<int8_t> (getBus(PC));
		}
		break;
	case 0x32: // LD [HL-], A
		writeBus(A, getHL());
		setHL(getHL() - 1);
		add_m_cycles(2);
		add_t_cycles(8);
		PC += 1;
		break;
	case 0xC3: // JP u16
		PC = get_next_two_bytes(PC);
		add_m_cycles(4);
		add_t_cycles(16);
		break;
	case 0xAF: // XOR A?
		A = A ^ A;
		NegativeFlag = !A;
		add_m_cycles(1);
		add_t_cycles(4);
		PC++;
		break;
		// MISC / UNGENERAL ENOUGH
	case 0xF3: // Disable Interrupts
		IME = false;
		add_m_cycles(1);
		add_t_cycles(4);
		PC++;
		break;
	case 0xC9: // RET
		PC = POP_STACK_16BIT();
		add_m_cycles(4);
		add_t_cycles(16);
		break;
	case 0xFA: // LD A, (u16)
		PC++;
		A = getBus(PC);
		A += getBus(PC) << 8;
		add_m_cycles(4);
		add_t_cycles(16);
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
