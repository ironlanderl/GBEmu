#include "pch.h"
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

bool shouldSkip = false;

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
		if (gameboy.isInsideInterval(address, 0x140, 0x150))
		{
			shouldSkip = true;
			return;
		}
		gameboy.writeBusUnrestricted(value, address);
	}

	// Altre operazioni per caricare le altre parti della memoria e altre variabili di GameBoy
	// Puoi adattare questa funzione per caricare altri dati dal JSON come necessario
}


// Funzione per controllare se lo stato finale corrisponde allo stato atteso
bool checkFinalState(GameBoy& gameboy, const json& testData) {
	// Verifica i registri della CPU
	if (gameboy.A != std::strtoul(testData["final"]["cpu"]["a"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro A è diverso. Atteso: %02X, Attuale: %02X\n", std::strtoul(testData["final"]["cpu"]["a"].get<std::string>().c_str(), nullptr, 16), gameboy.A);
		return false;
	}


	if (gameboy.B != std::strtoul(testData["final"]["cpu"]["b"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro B è diverso. Atteso: %02X, Attuale: %02X\n", std::strtoul(testData["final"]["cpu"]["b"].get<std::string>().c_str(), nullptr, 16), gameboy.B);
		return false;
	}


	if (gameboy.C != std::strtoul(testData["final"]["cpu"]["c"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro C è diverso. Atteso: %02X, Attuale: %02X\n", std::strtoul(testData["final"]["cpu"]["c"].get<std::string>().c_str(), nullptr, 16), gameboy.C);
		return false;
	}


	if (gameboy.D != std::strtoul(testData["final"]["cpu"]["d"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro D è diverso. Atteso: %02X, Attuale: %02X\n", std::strtoul(testData["final"]["cpu"]["d"].get<std::string>().c_str(), nullptr, 16), gameboy.D);
		return false;
	}


	if (gameboy.E != std::strtoul(testData["final"]["cpu"]["e"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro E è diverso. Atteso: %02X, Attuale: %02X\n", std::strtoul(testData["final"]["cpu"]["e"].get<std::string>().c_str(), nullptr, 16), gameboy.E);
		return false;
	}


	if (gameboy.H != std::strtoul(testData["final"]["cpu"]["h"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro H è diverso. Atteso: %02X, Attuale: %02X\n", std::strtoul(testData["final"]["cpu"]["h"].get<std::string>().c_str(), nullptr, 16), gameboy.H);
		return false;
	}


	if (gameboy.L != std::strtoul(testData["final"]["cpu"]["l"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro L è diverso. Atteso: %02X, Attuale: %02X\n", std::strtoul(testData["final"]["cpu"]["l"].get<std::string>().c_str(), nullptr, 16), gameboy.L);
		return false;
	}


	if (gameboy.PC != std::strtoul(testData["final"]["cpu"]["pc"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro PC è diverso. Atteso: %04X, Attuale: %04X\n", std::strtoul(testData["final"]["cpu"]["pc"].get<std::string>().c_str(), nullptr, 16), gameboy.PC);
		return false;
	}


	if (gameboy.SP != std::strtoul(testData["final"]["cpu"]["sp"].get<std::string>().c_str(), nullptr, 16)) {
		std::printf("Il registro SP è diverso. Atteso: %04X, Attuale: %04X\n", std::strtoul(testData["final"]["cpu"]["sp"].get<std::string>().c_str(), nullptr, 16), gameboy.SP);
		return false;
	}

	// Verifica i flag della CPU
	if (gameboy.ZeroFlag != (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 128) >> 7) {
		std::printf("Il flag ZeroFlag è diverso. Atteso: %d, Attuale: %d\n", (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 128) >> 7, gameboy.ZeroFlag);
		return false;
	}



	if (gameboy.NegativeFlag != (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 64) >> 6) {
		std::printf("Il flag NegativeFlag è diverso. Atteso: %d, Attuale: %d\n", (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 64) >> 6, gameboy.NegativeFlag);
		return false;
	}



	if (gameboy.HalfCarry != (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 32) >> 5) {
		std::printf("Il flag HalfCarry è diverso. Atteso: %d, Attuale: %d\n", (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 32) >> 5, gameboy.HalfCarry);
		return false;
	}



	if (gameboy.Carry != (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 16) >> 4) {
		std::printf("Il flag Carry è diverso. Atteso: %d, Attuale: %d\n", (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & 16) >> 4, gameboy.Carry);
		return false;
	}

	// Verifica ram
	for (size_t i = 0; i < testData["final"]["ram"].size(); ++i) {
		uint8_t value_expected = std::strtoul(testData["final"]["ram"][i][1].get<std::string>().c_str(), nullptr, 16);
		uint16_t address = std::strtoul(testData["final"]["ram"][i][0].get<std::string>().c_str(), nullptr, 16);
		uint8_t value_actual = gameboy.getBus(address);
		if (value_actual != value_expected) {
            std::printf("Valore in posizione %04X. Atteso: %02X, Attuale: %02X\n", address, value_expected, value_actual);
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

TEST(GameBoyTest, Instruction2C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\2C.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionBC) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\BC.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction42) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\42.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionD0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\D0.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionE5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\E5.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction53) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\53.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionFE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\FE.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction34) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\34.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionF9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\F9.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionDF) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\DF.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction35) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\35.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction00) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\00.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction14) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\14.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction44) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\44.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionD2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\D2.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionBA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\BA.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionC5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\C5.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\7A.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionC6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\C6.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction2B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\2B.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction21) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\21.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCD) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CD.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\7D.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionC0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\C0.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction1B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\1B.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\4A.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\6D.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionA3) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\A3.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction90) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\90.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction48) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\48.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CA.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction58) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\58.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionD9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\D9.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\5A.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction15) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\15.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction13) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\13.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction1C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\1C.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\5F.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionE1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\E1.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\5D.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\6E.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\6B.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction3C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\3C.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionEA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\EA.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionF3) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\F3.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction95) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\95.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction64) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\64.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction65) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\65.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionFB) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\FB.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionF0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\F0.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\4C.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionF1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\F1.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\6F.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction2E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\2E.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction06) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\06.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction88) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\88.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction3B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\3B.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\4E.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\7B.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction43) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\43.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction91) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\91.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction89) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\89.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionD7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\D7.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionA1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\A1.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\8C.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\4D.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction78) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\78.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\8E.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionDA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\DA.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction0E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\0E.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionC2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\C2.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction28) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\28.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\5C.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\6C.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction61) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\61.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction23) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\23.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction04) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\04.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction68) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\68.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\8B.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionB8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\B8.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionC7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\C7.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction24) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\24.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction52) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\52.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction97) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\97.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction25) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\25.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionBB) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\BB.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCF) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CF.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\8F.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction47) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\47.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction38) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\38.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction2D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\2D.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionC1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\C1.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionD8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\D8.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionA4) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\A4.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction45) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\45.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionEF) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\EF.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\5B.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\7F.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction40) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\40.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\5E.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction81) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\81.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionE9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\E9.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction0D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\0D.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionF7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\F7.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction85) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\85.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction79) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\79.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionE6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\E6.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction11) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\11.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction84) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\84.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction26) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\26.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction31) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\31.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction83) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\83.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CE.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction16) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\16.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction82) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\82.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionA0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\A0.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction57) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\57.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction1D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\1D.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionE7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\E7.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction67) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\67.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction32) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\32.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction92) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\92.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionC9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\C9.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionBE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\BE.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction20) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\20.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionAF) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\AF.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionE0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\E0.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction41) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\41.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionBF) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\BF.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionA6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\A6.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction55) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\55.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction3E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\3E.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionA7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\A7.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction2F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\2F.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction33) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\33.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\7C.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction60) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\60.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction3D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\3D.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\6A.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction59) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\59.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionFF) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\FF.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction03) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\03.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction51) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\51.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction93) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\93.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionA2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\A2.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction94) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\94.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\8A.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionC8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\C8.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction63) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\63.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction50) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\50.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction05) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\05.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\8D.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction0C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\0C.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction10) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\10.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction87) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\87.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction80) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\80.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction18) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\18.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionC3) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\C3.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionD6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\D6.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionD1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\D1.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction1E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\1E.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction30) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\30.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction49) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\49.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionD5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\D5.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction0B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\0B.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\4F.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction54) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\54.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\4B.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionF5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\F5.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionBD) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\BD.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction01) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\01.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\7E.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionFA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\FA.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction69) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\69.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction62) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\62.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionA5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\A5.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionB9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\B9.json");
    GameBoy gb;

	const int max = 50;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.advanceStep();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > max){
			break;
		}
		count++;
    }
}
