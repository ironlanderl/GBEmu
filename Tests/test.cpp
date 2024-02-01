#include "pch.h"
/*#include "../GBEmu/GameBoy.h"
#include "../GBEmu/GameBoy.cpp"

class GameBoyTest : public ::testing::Test {
public:
    GameBoy gb = GameBoy();

    // Helper function to execute an opcode and advance PC
    void executeAndAdvance(uint8_t opcode) {
        gb.writeBus(opcode, gb.PC);
        gb.advanceStep();
    }
};

TEST_F(GameBoyTest, Opcode_0x00) {
    // NOP - No operation, PC should be incremented
    executeAndAdvance(0x00);

    EXPECT_EQ(gb.PC, 0x01);
}

TEST_F(GameBoyTest, Opcode_0x05) {
    // DEC B - Decrement register B, check flags
    gb.B = 0x02;
    executeAndAdvance(0x05);

    EXPECT_EQ(gb.B, 0x01);
    EXPECT_EQ(gb.ZeroFlag, 0);  // Zero flag should not be set
    EXPECT_EQ(gb.NegativeFlag, 1);  // Negative flag should be set
    // Other flags should remain unchanged
}

TEST_F(GameBoyTest, Opcode_0x0D) {
    // DEC C - Decrement register C, check flags
    gb.C = 0x02;
    executeAndAdvance(0x0D);

    EXPECT_EQ(gb.C, 0x01);
    EXPECT_EQ(gb.ZeroFlag, 0);  // Zero flag should not be set
    EXPECT_EQ(gb.NegativeFlag, 1);  // Negative flag should be set
    // Other flags should remain unchanged
}

TEST_F(GameBoyTest, Opcode_0x0E) {
    // LD C, u8 - Load immediate value to register C
    executeAndAdvance(0x0E);
    EXPECT_EQ(gb.C, 0x00);  // Check if C is loaded with immediate value
    // Additional checks based on your specific implementation
}

TEST_F(GameBoyTest, Opcode_0x32) {
    // LD [HL-], A - Load value from register A to memory at HL and decrement HL
    gb.setHL(0x1000);
    gb.A = 0xAB;
    executeAndAdvance(0x32);

    EXPECT_EQ(gb.getBus(0x0FFF), 0xAB);  // Check if memory is updated correctly
    EXPECT_EQ(gb.getHL(), 0x0FFF);  // Check if HL is decremented
}

TEST_F(GameBoyTest, Opcode_0xC3) {
    // JP u16 - Jump to immediate 16-bit address
    executeAndAdvance(0xC3);

    EXPECT_EQ(gb.PC, gb.get_next_two_bytes(gb.PC));  // Check if PC is updated to the immediate 16-bit address
    // Additional checks based on your specific implementation
}

TEST_F(GameBoyTest, Opcode_0x20_JR_NZ_r8) {
    // JR NZ, r8 - Jump relative if ZeroFlag is not set
    gb.ZeroFlag = 0;  // Ensure ZeroFlag is not set
    executeAndAdvance(0x20);

    // Add expectations based on your specific implementation
    // Ensure PC is updated correctly based on the condition
}

// Add more test cases for the remaining opcodes...
*/