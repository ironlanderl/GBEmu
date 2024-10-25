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
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
	case 0x48:
	case 0x49:
	case 0x4A:
	case 0x4B:
	case 0x4C:
	case 0x4D:
	case 0x4E:
	case 0x4F:
	case 0x50:
	case 0x51:
	case 0x52:
	case 0x53:
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57:
	case 0x58:
	case 0x59:
	case 0x5A:
	case 0x5B:
	case 0x5C:
	case 0x5D:
	case 0x5E:
	case 0x5F:
	case 0x60:
	case 0x61:
	case 0x62:
	case 0x63:
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x67:
	case 0x68:
	case 0x69:
	case 0x6A:
	case 0x6B:
	case 0x6C:
	case 0x6D:
	case 0x6E:
	case 0x6F:
	case 0x70:
	case 0x71:
	case 0x72:
	case 0x73:
	case 0x74:
	case 0x75:
	case 0x76:
	case 0x77:
	case 0x78:
	case 0x79:
	case 0x7A:
	case 0x7B:
	case 0x7C:
	case 0x7D:
	case 0x7E:
	case 0x7F:
	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	case 0x84:
	case 0x85:
	case 0x86:
	case 0x87:
	case 0x88:
	case 0x89:
	case 0x8A:
	case 0x8B:
	case 0x8C:
	case 0x8D:
	case 0x8E:
	case 0x8F:
	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:
	case 0x98:
	case 0x99:
	case 0x9A:
	case 0x9B:
	case 0x9C:
	case 0x9D:
	case 0x9E:
	case 0x9F:
	case 0xA0:
	case 0xA1:
	case 0xA2:
	case 0xA3:
	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xA7:
	case 0xA8:
	case 0xA9:
	case 0xAA:
	case 0xAB:
	case 0xAC:
	case 0xAD:
	case 0xAE:
	case 0xAF:
	case 0xB0:
	case 0xB1:
	case 0xB2:
	case 0xB3:
	case 0xB4:
	case 0xB5:
	case 0xB6:
	case 0xB7:
	case 0xB8:
	case 0xB9:
	case 0xBA:
	case 0xBB:
	case 0xBC:
	case 0xBD:
	case 0xBE:
	case 0xBF:
	case 0xC0:
	case 0xC1:
	case 0xC2:
	case 0xC3:
	case 0xC4:
	case 0xC5:
	case 0xC6:
	case 0xC7:
	case 0xC8:
	case 0xC9:
	case 0xCA:
	case 0xCB:
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xCF:
	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:
	case 0xD4:
	case 0xD5:
	case 0xD6:
	case 0xD7:
	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF:
	case 0xE0:
	case 0xE1:
	case 0xE2:
	case 0xE3:
	case 0xE4:
	case 0xE5:
	case 0xE6:
	case 0xE7:
	case 0xE8:
	case 0xE9:
	case 0xEA:
	case 0xEB:
	case 0xEC:
	case 0xED:
	case 0xEE:
	case 0xEF:
	case 0xF0:
	case 0xF1:
	case 0xF2:
	case 0xF3:
	case 0xF4:
	case 0xF5:
	case 0xF6:
	case 0xF7:
	case 0xF8:
	case 0xF9:
	case 0xFA:
	case 0xFB:
	case 0xFC:
	case 0xFD:
	case 0xFE:
	case 0xFF:
		UnimplementedOpcode();
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

void GameBoy::sla(uint8_t& reg){
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

void GameBoy::sra(uint8_t& reg){
	Carry = reg & 1;
	uint8_t last_bit = reg >> 7 & 1;
	reg = reg >> 1 | last_bit << 7;
	ZeroFlag = reg == 0;
	NegativeFlag = false;
	HalfCarry = false;
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::sra_at_address(){
	uint8_t value = getBus(getHL());
	sra(value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::swap(uint8_t& reg){
	uint8_t lower = reg & 0xF;
	reg = reg >> 4 | lower << 4;
	ZeroFlag = reg == 0;
	NegativeFlag = false;
	HalfCarry = false;
	Carry = 0;
	add_t_cycles(8);
	add_m_cycles(2);
}
void GameBoy::swap_at_address(){
	uint8_t value = getBus(getHL());
	swap(value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}

void GameBoy::srl(uint8_t& reg){
	Carry = reg & 1;
	reg = reg >> 1;
	ZeroFlag = reg == 0;
	NegativeFlag = false;
	HalfCarry = false;
	add_t_cycles(8);
	add_m_cycles(2);
}
void GameBoy::srl_at_address(){
	uint8_t value = getBus(getHL());
	srl(value);
	writeBus(value, getHL());
	add_t_cycles(8);
	add_m_cycles(2);
}