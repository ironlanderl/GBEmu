#include <variant>

#include "GameBoy.h"

void GameBoy::UnimplementedOpcode()
{
	// throw new std::runtime_error("Unimplemented Opcode");
	std::printf("Unimplemented Opcode: %02X \n", getBus(PC));
}

void GameBoy::RunCBOpcode()
{
	PC++;
	switch (getBus(PC))
	{
	case 0x00:
		rlc(B);
		break;
	case 0x01:
		rlc(C);
		break;
	case 0x02:
		rlc(D);
		break;
	case 0x03:
		rlc(E);
		break;
	case 0x04:
		rlc(H);
		break;
	case 0x05:
		rlc(L);
		break;
	case 0x06:
		rlc_at_address();
		break;
	case 0x07:
		rlc(A);
		break;
	case 0x08:
		rrc(B);
		break;
	case 0x09:
		rrc(C);
		break;
	case 0x0A:
		rrc(D);
		break;
	case 0x0B:
		rrc(E);
		break;
	case 0x0C:
		rrc(H);
		break;
	case 0x0D:
		rrc(L);
		break;
	case 0x0E:
		rrc_at_address();
		break;
	case 0x0F:
		rrc(A);
		break;
	case 0x10:
		rl(B);
		break;
	case 0x11:
		rl(C);
		break;
	case 0x12:
		rl(D);
		break;
	case 0x13:
		rl(E);
		break;
	case 0x14:
		rl(H);
		break;
	case 0x15:
		rl(L);
		break;
	case 0x16:
		rl_at_address();
		break;
	case 0x17:
		rl(A);
		break;
	case 0x18:
		rr(B);
		break;
	case 0x19:
		rr(C);
		break;
	case 0x1A:
		rr(D);
		break;
	case 0x1B:
		rr(E);
		break;
	case 0x1C:
		rr(H);
		break;
	case 0x1D:
		rr(L);
		break;
	case 0x1E:
		rr_at_address();
		break;
	case 0x1F:
		rr(A);
		break;
	case 0x20:
		sla(B);
		break;
	case 0x21:
		sla(C);
		break;
	case 0x22:
		sla(D);
		break;
	case 0x23:
		sla(E);
		break;
	case 0x24:
		sla(H);
		break;
	case 0x25:
		sla(L);
		break;
	case 0x26:
		sla_at_address();
		break;
	case 0x27:
		sla(A);
		break;
	case 0x28:
		sra(B);
		break;
	case 0x29:
		sra(C);
		break;
	case 0x2A:
		sra(D);
		break;
	case 0x2B:
		sra(E);
		break;
	case 0x2C:
		sra(H);
		break;
	case 0x2D:
		sra(L);
		break;
	case 0x2E:
		sra_at_address();
		break;
	case 0x2F:
		sra(A);
		break;
	case 0x30:
		swap(B);
		break;
	case 0x31:
		swap(C);
		break;
	case 0x32:
		swap(D);
		break;
	case 0x33:
		swap(E);
		break;
	case 0x34:
		swap(H);
		break;
	case 0x35:
		swap(L);
		break;
	case 0x36:
		swap_at_address();
		break;
	case 0x37:
		swap(A);
		break;
	case 0x38:
		srl(B);
		break;
	case 0x39:
		srl(C);
		break;
	case 0x3A:
		srl(D);
		break;
	case 0x3B:
		srl(E);
		break;
	case 0x3C:
		srl(H);
		break;
	case 0x3D:
		srl(L);
		break;
	case 0x3E:
		srl_at_address();
		break;
	case 0x3F:
		srl(A);
		break;
	case 0x40:
		bit_check(0, B);
		break;
	case 0x41:
		bit_check(0, C);
		break;
	case 0x42:
		bit_check(0, D);
		break;
	case 0x43:
		bit_check(0, E);
		break;
	case 0x44:
		bit_check(0, H);
		break;
	case 0x45:
		bit_check(0, L);
		break;
	case 0x46:
		bit_check_at_address(0);
		break;
	case 0x47:
		bit_check(0, A);
		break;
	case 0x48:
		bit_check(1, B);
		break;
	case 0x49:
		bit_check(1, C);
		break;
	case 0x4A:
		bit_check(1, D);
		break;
	case 0x4B:
		bit_check(1, E);
		break;
	case 0x4C:
		bit_check(1, H);
		break;
	case 0x4D:
		bit_check(1, L);
		break;
	case 0x4E:
		bit_check_at_address(1);
		break;
	case 0x4F:
		bit_check(1, A);
		break;
	case 0x50:
		bit_check(2, B);
		break;
	case 0x51:
		bit_check(2, C);
		break;
	case 0x52:
		bit_check(2, D);
		break;
	case 0x53:
		bit_check(2, E);
		break;
	case 0x54:
		bit_check(2, H);
		break;
	case 0x55:
		bit_check(2, L);
		break;
	case 0x56:
		bit_check_at_address(2);
		break;
	case 0x57:
		bit_check(2, A);
		break;
	case 0x58:
		bit_check(3, B);
		break;
	case 0x59:
		bit_check(3, C);
		break;
	case 0x5A:
		bit_check(3, D);
		break;
	case 0x5B:
		bit_check(3, E);
		break;
	case 0x5C:
		bit_check(3, H);
		break;
	case 0x5D:
		bit_check(3, L);
		break;
	case 0x5E:
		bit_check_at_address(3);
		break;
	case 0x5F:
		bit_check(3, A);
		break;
	case 0x60:
		bit_check(4, B);
		break;
	case 0x61:
		bit_check(4, C);
		break;
	case 0x62:
		bit_check(4, D);
		break;
	case 0x63:
		bit_check(4, E);
		break;
	case 0x64:
		bit_check(4, H);
		break;
	case 0x65:
		bit_check(4, L);
		break;
	case 0x66:
		bit_check_at_address(4);
		break;
	case 0x67:
		bit_check(4, A);
		break;
	case 0x68:
		bit_check(5, B);
		break;
	case 0x69:
		bit_check(5, C);
		break;
	case 0x6A:
		bit_check(5, D);
		break;
	case 0x6B:
		bit_check(5, E);
		break;
	case 0x6C:
		bit_check(5, H);
		break;
	case 0x6D:
		bit_check(5, L);
		break;
	case 0x6E:
		bit_check_at_address(5);
		break;
	case 0x6F:
		bit_check(5, A);
		break;
	case 0x70:
		bit_check(6, B);
		break;
	case 0x71:
		bit_check(6, C);
		break;
	case 0x72:
		bit_check(6, D);
		break;
	case 0x73:
		bit_check(6, E);
		break;
	case 0x74:
		bit_check(6, H);
		break;
	case 0x75:
		bit_check(6, L);
		break;
	case 0x76:
		bit_check_at_address(6);
		break;
	case 0x77:
		bit_check(6, A);
		break;
	case 0x78:
		bit_check(7, B);
		break;
	case 0x79:
		bit_check(7, C);
		break;
	case 0x7A:
		bit_check(7, D);
		break;
	case 0x7B:
		bit_check(7, E);
		break;
	case 0x7C:
		bit_check(7, H);
		break;
	case 0x7D:
		bit_check(7, L);
		break;
	case 0x7E:
		bit_check_at_address(7);
		break;
	case 0x7F:
		bit_check(7, A);
		break;
	case 0x80:
		bit_reset(0, B);
		break;
	case 0x81:
		bit_reset(0, C);
		break;
	case 0x82:
		bit_reset(0, D);
		break;
	case 0x83:
		bit_reset(0, E);
		break;
	case 0x84:
		bit_reset(0, H);
		break;
	case 0x85:
		bit_reset(0, L);
		break;
	case 0x86:
		bit_reset_at_address(0);
		break;
	case 0x87:
		bit_reset(0, A);
		break;
	case 0x88:
		bit_reset(1, B);
		break;
	case 0x89:
		bit_reset(1, C);
		break;
	case 0x8A:
		bit_reset(1, D);
		break;
	case 0x8B:
		bit_reset(1, E);
		break;
	case 0x8C:
		bit_reset(1, H);
		break;
	case 0x8D:
		bit_reset(1, L);
		break;
	case 0x8E:
		bit_reset_at_address(1);
		break;
	case 0x8F:
		bit_reset(1, A);
		break;
	case 0x90:
		bit_reset(2, B);
		break;
	case 0x91:
		bit_reset(2, C);
		break;
	case 0x92:
		bit_reset(2, D);
		break;
	case 0x93:
		bit_reset(2, E);
		break;
	case 0x94:
		bit_reset(2, H);
		break;
	case 0x95:
		bit_reset(2, L);
		break;
	case 0x96:
		bit_reset_at_address(2);
		break;
	case 0x97:
		bit_reset(2, A);
		break;
	case 0x98:
		bit_reset(3, B);
		break;
	case 0x99:
		bit_reset(3, C);
		break;
	case 0x9A:
		bit_reset(3, D);
		break;
	case 0x9B:
		bit_reset(3, E);
		break;
	case 0x9C:
		bit_reset(3, H);
		break;
	case 0x9D:
		bit_reset(3, L);
		break;
	case 0x9E:
		bit_reset_at_address(3);
		break;
	case 0x9F:
		bit_reset(3, A);
		break;
	case 0xA0:
		bit_reset(4, B);
		break;
	case 0xA1:
		bit_reset(4, C);
		break;
	case 0xA2:
		bit_reset(4, D);
		break;
	case 0xA3:
		bit_reset(4, E);
		break;
	case 0xA4:
		bit_reset(4, H);
		break;
	case 0xA5:
		bit_reset(4, L);
		break;
	case 0xA6:
		bit_reset_at_address(4);
		break;
	case 0xA7:
		bit_reset(4, A);
		break;
	case 0xA8:
		bit_reset(5, B);
		break;
	case 0xA9:
		bit_reset(5, C);
		break;
	case 0xAA:
		bit_reset(5, D);
		break;
	case 0xAB:
		bit_reset(5, E);
		break;
	case 0xAC:
		bit_reset(5, H);
		break;
	case 0xAD:
		bit_reset(5, L);
		break;
	case 0xAE:
		bit_reset_at_address(5);
		break;
	case 0xAF:
		bit_reset(5, A);
		break;
	case 0xB0:
		bit_reset(6, B);
		break;
	case 0xB1:
		bit_reset(6, C);
		break;
	case 0xB2:
		bit_reset(6, D);
		break;
	case 0xB3:
		bit_reset(6, E);
		break;
	case 0xB4:
		bit_reset(6, H);
		break;
	case 0xB5:
		bit_reset(6, L);
		break;
	case 0xB6:
		bit_reset_at_address(6);
		break;
	case 0xB7:
		bit_reset(6, A);
		break;
	case 0xB8:
		bit_reset(7, B);
		break;
	case 0xB9:
		bit_reset(7, C);
		break;
	case 0xBA:
		bit_reset(7, D);
		break;
	case 0xBB:
		bit_reset(7, E);
		break;
	case 0xBC:
		bit_reset(7, H);
		break;
	case 0xBD:
		bit_reset(7, L);
		break;
	case 0xBE:
		bit_reset_at_address(7);
		break;
	case 0xBF:
		bit_reset(7, A);
		break;
	case 0xC0:
		bit_set(0, B);
		break;
	case 0xC1:
		bit_set(0, C);
		break;
	case 0xC2:
		bit_set(0, D);
		break;
	case 0xC3:
		bit_set(0, E);
		break;
	case 0xC4:
		bit_set(0, H);
		break;
	case 0xC5:
		bit_set(0, L);
		break;
	case 0xC6:
		bit_set_at_address(0);
		break;
	case 0xC7:
		bit_set(0, A);
		break;
	case 0xC8:
		bit_set(1, B);
		break;
	case 0xC9:
		bit_set(1, C);
		break;
	case 0xCA:
		bit_set(1, D);
		break;
	case 0xCB:
		bit_set(1, E);
		break;
	case 0xCC:
		bit_set(1, H);
		break;
	case 0xCD:
		bit_set(1, L);
		break;
	case 0xCE:
		bit_set_at_address(1);
		break;
	case 0xCF:
		bit_set(1, A);
		break;
	case 0xD0:
		bit_set(2, B);
		break;
	case 0xD1:
		bit_set(2, C);
		break;
	case 0xD2:
		bit_set(2, D);
		break;
	case 0xD3:
		bit_set(2, E);
		break;
	case 0xD4:
		bit_set(2, H);
		break;
	case 0xD5:
		bit_set(2, L);
		break;
	case 0xD6:
		bit_set_at_address(2);
		break;
	case 0xD7:
		bit_set(2, A);
		break;
	case 0xD8:
		bit_set(3, B);
		break;
	case 0xD9:
		bit_set(3, C);
		break;
	case 0xDA:
		bit_set(3, D);
		break;
	case 0xDB:
		bit_set(3, E);
		break;
	case 0xDC:
		bit_set(3, H);
		break;
	case 0xDD:
		bit_set(3, L);
		break;
	case 0xDE:
		bit_set_at_address(3);
		break;
	case 0xDF:
		bit_set(3, A);
		break;
	case 0xE0:
		bit_set(4, B);
		break;
	case 0xE1:
		bit_set(4, C);
		break;
	case 0xE2:
		bit_set(4, D);
		break;
	case 0xE3:
		bit_set(4, E);
		break;
	case 0xE4:
		bit_set(4, H);
		break;
	case 0xE5:
		bit_set(4, L);
		break;
	case 0xE6:
		bit_set_at_address(4);
		break;
	case 0xE7:
		bit_set(4, A);
		break;
	case 0xE8:
		bit_set(5, B);
		break;
	case 0xE9:
		bit_set(5, C);
		break;
	case 0xEA:
		bit_set(5, D);
		break;
	case 0xEB:
		bit_set(5, E);
		break;
	case 0xEC:
		bit_set(5, H);
		break;
	case 0xED:
		bit_set(5, L);
		break;
	case 0xEE:
		bit_set_at_address(5);
		break;
	case 0xEF:
		bit_set(5, A);
		break;
	case 0xF0:
		bit_set(6, B);
		break;
	case 0xF1:
		bit_set(6, C);
		break;
	case 0xF2:
		bit_set(6, D);
		break;
	case 0xF3:
		bit_set(6, E);
		break;
	case 0xF4:
		bit_set(6, H);
		break;
	case 0xF5:
		bit_set(6, L);
		break;
	case 0xF6:
		bit_set_at_address(6);
		break;
	case 0xF7:
		bit_set(6, A);
		break;
	case 0xF8:
		bit_set(7, B);
		break;
	case 0xF9:
		bit_set(7, C);
		break;
	case 0xFA:
		bit_set(7, D);
		break;
	case 0xFB:
		bit_set(7, E);
		break;
	case 0xFC:
		bit_set(7, H);
		break;
	case 0xFD:
		bit_set(7, L);
		break;
	case 0xFE:
		bit_set_at_address(7);
		break;
	case 0xFF:
		bit_set(7, A);
		break;
	default:
		std::printf("Invalid opcode: %02X \n", getBus(PC));
		status = PAUSED;
		// exit(-1);
	}
	PC++;
}

void GameBoy::rlc(uint8_t &reg)
{
	uint8_t carry = reg >> 7;
	reg = (reg << 1) | carry;
	ZeroFlag = reg == 0;
	NegativeFlag = false;
	HalfCarry = false;
	Carry = carry;
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::rlc_at_address()
{
	uint8_t value = getBus(getHL());
	rlc(value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::rrc(uint8_t &reg)
{
	uint8_t carry = reg & 0x1;
	reg = (carry << 7) | (reg >> 1);
	ZeroFlag = reg == 0;
	NegativeFlag = false;
	HalfCarry = false;
	Carry = carry;
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::rrc_at_address()
{
	uint8_t value = getBus(getHL());
	rrc(value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::rl(uint8_t &reg)
{
	uint8_t carry = reg >> 7;
	reg = (reg << 1) | Carry;
	ZeroFlag = reg == 0;
	NegativeFlag = false;
	HalfCarry = false;
	Carry = carry;
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::rl_at_address()
{
	uint8_t value = getBus(getHL());
	rl(value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::rr(uint8_t &reg)
{
	uint8_t carry = reg & 0x1;
	reg = (Carry << 7) | (reg >> 1);
	ZeroFlag = reg == 0;
	NegativeFlag = false;
	HalfCarry = false;
	Carry = carry;
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::rr_at_address()
{
	uint8_t value = getBus(getHL());
	rr(value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::sla(uint8_t &reg)
{
	Carry = reg >> 7 & 1;
	reg = reg << 1;
	ZeroFlag = reg == 0;
	NegativeFlag = false;
	HalfCarry = false;
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::sla_at_address()
{
	uint8_t value = getBus(getHL());
	sla(value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::sra(uint8_t &reg)
{
	Carry = reg & 1;
	uint8_t last_bit = reg >> 7 & 1;
	reg = reg >> 1 | last_bit << 7;
	ZeroFlag = reg == 0;
	NegativeFlag = false;
	HalfCarry = false;
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::sra_at_address()
{
	uint8_t value = getBus(getHL());
	sra(value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::swap(uint8_t &reg)
{
	uint8_t lower = reg & 0xF;
	reg = reg >> 4 | lower << 4;
	ZeroFlag = reg == 0;
	NegativeFlag = false;
	HalfCarry = false;
	Carry = 0;
	add_t_cycles(8);
	add_m_cycles(2);
}
void GameBoy::swap_at_address()
{
	uint8_t value = getBus(getHL());
	swap(value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::srl(uint8_t &reg)
{
	Carry = reg & 1;
	reg = reg >> 1;
	ZeroFlag = reg == 0;
	NegativeFlag = false;
	HalfCarry = false;
	add_t_cycles(8);
	add_m_cycles(2);
}
void GameBoy::srl_at_address()
{
	uint8_t value = getBus(getHL());
	srl(value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::bit_check(uint8_t bit, uint8_t &reg)
{
	NegativeFlag = false;
	HalfCarry = true;
	ZeroFlag = (reg & (0x1 << bit)) == 0;
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::bit_check_at_address(uint8_t bit)
{
	uint8_t value = getBus(getHL());
	bit_check(bit, value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::bit_reset(uint8_t bit, uint8_t &reg)
{
	reg = reg & ~(1 << bit);
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::bit_reset_at_address(uint8_t bit)
{
	uint8_t value = getBus(getHL());
	bit_reset(bit, value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::bit_set(uint8_t bit, uint8_t &reg)
{
	reg = reg | (1 << bit);
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::bit_set_at_address(uint8_t bit)
{
	uint8_t value = getBus(getHL());
	bit_set(bit, value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}