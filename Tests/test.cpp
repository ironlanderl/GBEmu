#include "pch.h"
#include "../GBEmu/GameBoy.h"
//#include "../GBEmu/Opcodes.cpp"
//#include "../GBEmu/GameBoy.cpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include <cstdlib> // For std::strtoul
/*
using json = nlohmann::json;
namespace fs = boost::filesystem;

// Definizione della struttura per la memoria RAM
struct RAMEntry {
    std::string address;
    std::string value;
};

void loadTestDataToGameBoy(GameBoy& gameboy, json data) {

    // Set mapper to zero
    gameboy.ROM[0x147] = 0;

    // Carica i dati nel GameBoy
    // Carica i registri della CPU
    gameboy.A = std::strtoul(data["initial"]["cpu"]["a"].get<std::string>().c_str(), nullptr, 16);
    gameboy.B = std::strtoul(data["initial"]["cpu"]["b"].get<std::string>().c_str(), nullptr, 16);
    gameboy.C = std::strtoul(data["initial"]["cpu"]["c"].get<std::string>().c_str(), nullptr, 16);
    gameboy.D = std::strtoul(data["initial"]["cpu"]["d"].get<std::string>().c_str(), nullptr, 16);
    gameboy.E = std::strtoul(data["initial"]["cpu"]["e"].get<std::string>().c_str(), nullptr, 16);
    gameboy.H = std::strtoul(data["initial"]["cpu"]["h"].get<std::string>().c_str(), nullptr, 16);
    gameboy.L = std::strtoul(data["initial"]["cpu"]["l"].get<std::string>().c_str(), nullptr, 16);
    gameboy.PC = std::strtoul(data["initial"]["cpu"]["pc"].get<std::string>().c_str(), nullptr, 16);
    gameboy.SP = std::strtoul(data["initial"]["cpu"]["sp"].get<std::string>().c_str(), nullptr, 16);
    uint8_t f = std::strtoul(data["initial"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16);
    gameboy.ZeroFlag = (f & 0x80) >> 7;
    gameboy.NegativeFlag = (f & 0x40) >> 6;
    gameboy.HalfCarry = (f & 0x20) >> 5;
    gameboy.Carry = (f & 0x10) >> 4;

    // Carica la memoria ROM
    // Assumiamo che ci sia un array "rom" nel JSON contenente i dati ROM
    for (size_t i = 0; i < data["initial"]["ram"].size(); ++i) {
        uint8_t value = std::strtoul(data["initial"]["ram"][i][1].get<std::string>().c_str(), nullptr, 16);
        uint16_t address = std::strtoul(data["initial"]["ram"][i][0].get<std::string>().c_str(), nullptr, 16);
        gameboy.writeBusUnrestricted(value, address);
    }

    // Altre operazioni per caricare le altre parti della memoria e altre variabili di GameBoy
    // Puoi adattare questa funzione per caricare altri dati dal JSON come necessario
}


// Funzione per controllare se lo stato finale corrisponde allo stato atteso
bool checkFinalState(GameBoy& gameboy, const json& testData) {
    // Verifica i registri della CPU
    if (gameboy.A != std::strtoul(testData["final"]["cpu"]["a"].get<std::string>().c_str(), nullptr, 16) ||
        gameboy.B != std::strtoul(testData["final"]["cpu"]["b"].get<std::string>().c_str(), nullptr, 16) ||
        gameboy.C != std::strtoul(testData["final"]["cpu"]["c"].get<std::string>().c_str(), nullptr, 16) ||
        gameboy.D != std::strtoul(testData["final"]["cpu"]["d"].get<std::string>().c_str(), nullptr, 16) ||
        gameboy.E != std::strtoul(testData["final"]["cpu"]["e"].get<std::string>().c_str(), nullptr, 16) ||
        gameboy.H != std::strtoul(testData["final"]["cpu"]["h"].get<std::string>().c_str(), nullptr, 16) ||
        gameboy.L != std::strtoul(testData["final"]["cpu"]["l"].get<std::string>().c_str(), nullptr, 16) ||
        gameboy.PC != std::strtoul(testData["final"]["cpu"]["pc"].get<std::string>().c_str(), nullptr, 16) ||
        gameboy.SP != std::strtoul(testData["final"]["cpu"]["sp"].get<std::string>().c_str(), nullptr, 16)) {
        return false; // Se anche uno dei registri non corrisponde, restituisci falso
    }

    // Verifica i flag della CPU
    uint8_t f_expected = std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16);
    uint8_t f_actual = (gameboy.ZeroFlag << 7) | (gameboy.NegativeFlag << 6) | (gameboy.HalfCarry << 5) | (gameboy.Carry << 4);
    if (f_actual != f_expected) {
        return false;
    }

    // Verifica ram
    for (size_t i = 0; i < testData["final"]["ram"].size(); ++i) {
        uint8_t value_expected = std::strtoul(testData["final"]["ram"][i][1].get<std::string>().c_str(), nullptr, 16);
        uint16_t address = std::strtoul(testData["final"]["ram"][i][0].get<std::string>().c_str(), nullptr, 16);
        uint8_t value_actual = gameboy.getBus(address);
        if (value_actual != value_expected) {
            return false;
        }
    }

    // Altre verifiche di stato finale come necessario
    // Puoi aggiungere qui ulteriori controlli di stato finale se necessario

    return true; // Se tutti i controlli passano, restituisci true
}




json loadTestData(std::string filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Impossibile aprire il file: " << filepath << " - Motivo: " << strerror(errno) << std::endl;
        throw std::runtime_error("Errore durante l'apertura del file.");
    }

    json data;
    file >> data;
    file.close();

    return data;
}

// Sanity Check
TEST(GameBoyTest, SanityCheck) {
    EXPECT_EQ(true, true);
}

TEST(GameBoyTest, Instruction00) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\00.json");
    GameBoy gb;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction
        gb.execute_step();

        // Start check
        bool isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);
    }
}

TEST(GameBoyTest, Instruction01) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\01.json");
    GameBoy gb;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction
        gb.execute_step();

        // Start check
        bool isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);
    }
}
*/
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
