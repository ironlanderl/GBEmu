#include "GameBoy.h"

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
		return ROM[address];
	}
	else if (isInsideInterval(address, 0x8000, 0x9FFF))
	{
		return VRAM[address]; // TODO
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
		add_m_cycles(1);
		add_t_cycles(4);
		PC++;
		break;
	case 0x05: // DEC B
		HalfCarry = (((B & 0xF) + (-1 & 0xF)) & 0x10) == 0x10;
		B--;
		add_m_cycles(1);
		add_t_cycles(4);
		PC++;
		NegativeFlag = 1;
		ZeroFlag = !B;
		break;
	case 0x06: // LD b, u8
		B = getBus(PC + 1);
		add_m_cycles(2);
		add_t_cycles(8);
		PC += 2;
		break;
	case 0x0D: // DEC C
		HalfCarry = (((C & 0xF) + (-1 & 0xF)) & 0x10) == 0x10;
		C--;
		add_m_cycles(1);
		add_t_cycles(4);
		PC++;
		NegativeFlag = 1;
		ZeroFlag = !C;
		break;
	case 0x0E: // LD c, u8
		C = getBus(PC + 1);
		add_m_cycles(2);
		add_t_cycles(8);
		PC += 2;
		break;
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
			PC += 2; // TODO: FIX
			PC += static_cast<int8_t> (get_next_byte(PC));
		}
		break;
	case 0x21: // LD HL, u16
		setHL(get_next_two_bytes(PC));
		add_m_cycles(3);
		add_t_cycles(12);
		PC += 3;
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
	case 0xAF:
		A = A ^ A;
		NegativeFlag = !A;
		add_m_cycles(1);
		add_t_cycles(4);
		PC++;
		break;
	default:
		std::printf("Invalid opcode: %02X", getBus(PC));
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

uint16_t GameBoy::getHL()
{
	return ((H << 8) | L);
}

void GameBoy::setHL(uint16_t value)
{
	H = value >> 8;
	L = value & 0xFF;
}
