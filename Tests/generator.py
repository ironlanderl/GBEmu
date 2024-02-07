import os
import re

# Define the header and source file content template
header_content = """#include "pch.h"
#include "../GBEmu/GameBoy.h"
#include "../GBEmu/Opcodes.cpp"
#include "../GBEmu/GameBoy.cpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include <cstdlib> // For std::strtoul

using json = nlohmann::json;
namespace fs = boost::filesystem;

//bool shouldSkip = false;

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
		// Kind of a cop-out, but if writing near the mapper selector, we skip
		/*if (gameboy.isInsideInterval(address, 0x140, 0x150))
		{
			shouldSkip = true;
			return;
		}*/
		gameboy.writeBusUnrestricted(value, address);
	}

	// Altre operazioni per caricare le altre parti della memoria e altre variabili di GameBoy
	// Puoi adattare questa funzione per caricare altri dati dal JSON come necessario
}


// Funzione per controllare se lo stato finale corrisponde allo stato atteso
bool checkFinalState(GameBoy& gameboy, const json& testData) {
	// Verifica i registri della CPU
	if (gameboy.A != std::strtoul(testData["final"]["cpu"]["a"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro A è diverso. Atteso: %02X, Attuale: %02X\\n", std::strtoul(testData["final"]["cpu"]["a"].get<std::string>().c_str(), nullptr, 16), gameboy.A);
		return false;
	}


	if (gameboy.B != std::strtoul(testData["final"]["cpu"]["b"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro B è diverso. Atteso: %02X, Attuale: %02X\\n", std::strtoul(testData["final"]["cpu"]["b"].get<std::string>().c_str(), nullptr, 16), gameboy.B);
		return false;
	}


	if (gameboy.C != std::strtoul(testData["final"]["cpu"]["c"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro C è diverso. Atteso: %02X, Attuale: %02X\\n", std::strtoul(testData["final"]["cpu"]["c"].get<std::string>().c_str(), nullptr, 16), gameboy.C);
		return false;
	}


	if (gameboy.D != std::strtoul(testData["final"]["cpu"]["d"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro D è diverso. Atteso: %02X, Attuale: %02X\\n", std::strtoul(testData["final"]["cpu"]["d"].get<std::string>().c_str(), nullptr, 16), gameboy.D);
		return false;
	}


	if (gameboy.E != std::strtoul(testData["final"]["cpu"]["e"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro E è diverso. Atteso: %02X, Attuale: %02X\\n", std::strtoul(testData["final"]["cpu"]["e"].get<std::string>().c_str(), nullptr, 16), gameboy.E);
		return false;
	}


	if (gameboy.H != std::strtoul(testData["final"]["cpu"]["h"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro H è diverso. Atteso: %02X, Attuale: %02X\\n", std::strtoul(testData["final"]["cpu"]["h"].get<std::string>().c_str(), nullptr, 16), gameboy.H);
		return false;
	}


	if (gameboy.L != std::strtoul(testData["final"]["cpu"]["l"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro L è diverso. Atteso: %02X, Attuale: %02X\\n", std::strtoul(testData["final"]["cpu"]["l"].get<std::string>().c_str(), nullptr, 16), gameboy.L);
		return false;
	}


	if (gameboy.PC != std::strtoul(testData["final"]["cpu"]["pc"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro PC è diverso. Atteso: %04X, Attuale: %04X\\n", std::strtoul(testData["final"]["cpu"]["pc"].get<std::string>().c_str(), nullptr, 16), gameboy.PC);
		return false;
	}


	if (gameboy.SP != std::strtoul(testData["final"]["cpu"]["sp"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro SP è diverso. Atteso: %04X, Attuale: %04X\\n", std::strtoul(testData["final"]["cpu"]["sp"].get<std::string>().c_str(), nullptr, 16), gameboy.SP);
		return false;
	}

	// Verifica i flag della CPU
	if (gameboy.ZeroFlag != (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 128) >> 7) {
		std::printf("Il flag ZeroFlag è diverso. Atteso: %d, Attuale: %d\\n", (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 128) >> 7, gameboy.ZeroFlag);
		return false;
	}



	if (gameboy.NegativeFlag != (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 64) >> 6) {
		std::printf("Il flag NegativeFlag è diverso. Atteso: %d, Attuale: %d\\n", (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 64) >> 6, gameboy.NegativeFlag);
		return false;
	}



	if (gameboy.HalfCarry != (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 32) >> 5) {
		std::printf("Il flag HalfCarry è diverso. Atteso: %d, Attuale: %d\\n", (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 32) >> 5, gameboy.HalfCarry);
		return false;
	}



	if (gameboy.Carry != (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 16) >> 4) {
		std::printf("Il flag Carry è diverso. Atteso: %d, Attuale: %d\\n", (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 16) >> 4, gameboy.Carry);
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
"""

test_case_template = """TEST(GameBoyTest, Instruction{0}) {{
    json data = loadTestData("C:\\\\Users\\\\fabri\\\\source\\\\GBEmu\\\\test_data\\\\{1}.json");
    GameBoy gb;

    for (const auto& test : data) {{
        // debug
        std::printf("Running Test: %s\\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({{}}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        /*if (shouldSkip) {{
            isItOk = true;
            shouldSkip = false;
        }}
        else {{
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }}*/
        
        gb.advanceStep();
        isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);
    }}
}}
"""

# Function to extract opcodes from source files
def extract_opcodes_from_files(files):
    opcodes = set()
    for file in files:
        with open(file, 'r') as f:
            content = f.read()
            opcodes |= set(re.findall(r'case 0x([0-9A-F]{2}):', content))
    return opcodes

# Provide the path to the directory containing source files
source_files_path = "path/to/source/files"

# Get all C++ source files in the directory
source_files = ["C:/Users/fabri/source/GBEmu/GBEmu/GameBoy.cpp"]

# Extract opcodes from source files
implemented_opcodes = extract_opcodes_from_files(source_files)

# Create the header file
with open("generated_tests.cpp", "w") as file:
    # Write header content
    file.write(header_content)

    # Write test case for each opcode
    for opcode in implemented_opcodes:
        file.write("\n")
        file.write(test_case_template.format(opcode, opcode))
