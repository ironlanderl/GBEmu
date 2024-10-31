
#include "GameBoy.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <gtest/gtest.h>
#include <cstdlib> // For std::strtoul

using json = nlohmann::json;

const int MAX = 10;

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

TEST(GameBoyTest, Instruction00) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/00.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction01) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/01.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction02) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/02.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction03) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/03.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction04) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/04.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction05) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/05.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction06) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/06.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction07) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/07.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction08) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/08.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction09) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/09.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction0a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/0a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction0b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/0b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction0c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/0c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction0d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/0d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction0e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/0e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction0f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/0f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction10) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/10.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction11) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/11.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction12) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/12.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction13) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/13.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction14) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/14.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction15) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/15.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction16) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/16.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction17) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/17.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction18) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/18.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction19) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/19.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction1a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/1a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction1b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/1b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction1c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/1c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction1d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/1d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction1e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/1e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction1f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/1f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction20) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/20.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction21) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/21.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction22) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/22.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction23) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/23.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction24) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/24.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction25) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/25.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction26) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/26.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction27) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/27.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction28) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/28.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction29) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/29.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction2a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/2a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction2b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/2b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction2c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/2c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction2d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/2d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction2e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/2e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction2f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/2f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction30) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/30.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction31) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/31.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction32) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/32.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction33) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/33.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction34) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/34.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction35) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/35.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction36) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/36.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction37) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/37.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction38) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/38.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction39) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/39.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction3a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/3a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction3b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/3b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction3c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/3c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction3d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/3d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction3e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/3e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction3f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/3f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction40) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/40.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction41) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/41.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction42) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/42.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction43) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/43.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction44) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/44.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction45) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/45.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction46) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/46.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction47) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/47.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction48) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/48.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction49) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/49.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/4a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/4b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/4c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/4d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/4e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction4f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/4f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction50) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/50.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction51) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/51.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction52) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/52.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction53) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/53.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction54) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/54.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction55) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/55.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction56) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/56.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction57) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/57.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction58) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/58.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction59) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/59.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/5a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/5b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/5c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/5d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/5e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction5f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/5f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction60) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/60.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction61) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/61.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction62) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/62.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction63) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/63.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction64) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/64.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction65) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/65.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction66) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/66.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction67) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/67.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction68) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/68.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction69) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/69.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/6a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/6b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/6c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/6d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/6e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction6f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/6f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction70) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/70.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction71) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/71.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction72) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/72.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction73) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/73.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction74) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/74.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction75) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/75.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction76) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/76.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction77) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/77.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction78) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/78.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction79) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/79.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/7a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/7b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/7c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/7d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/7e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction7f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/7f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction80) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/80.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction81) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/81.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction82) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/82.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction83) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/83.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction84) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/84.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction85) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/85.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction86) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/86.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction87) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/87.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction88) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/88.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction89) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/89.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/8a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/8b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/8c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/8d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/8e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction8f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/8f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction90) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/90.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction91) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/91.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction92) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/92.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction93) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/93.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction94) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/94.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction95) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/95.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction96) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/96.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction97) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/97.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction98) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/98.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction99) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/99.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction9a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/9a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction9b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/9b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction9c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/9c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction9d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/9d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction9e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/9e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instruction9f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/9f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiona0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/a0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiona1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/a1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiona2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/a2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiona3) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/a3.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiona4) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/a4.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiona5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/a5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiona6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/a6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiona7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/a7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiona8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/a8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiona9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/a9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionaa) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/aa.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionab) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/ab.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionac) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/ac.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionad) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/ad.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionae) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/ae.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionaf) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/af.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionb0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/b0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionb1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/b1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionb2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/b2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionb3) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/b3.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionb4) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/b4.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionb5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/b5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionb6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/b6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionb7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/b7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionb8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/b8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionb9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/b9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionba) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/ba.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionbb) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/bb.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionbc) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/bc.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionbd) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/bd.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionbe) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/be.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionbf) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/bf.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionc0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/c0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionc1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/c1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionc2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/c2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionc3) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/c3.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionc4) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/c4.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionc5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/c5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionc6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/c6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionc7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/c7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionc8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/c8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionc9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/c9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionca) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/ca.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructioncc) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/cc.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructioncd) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/cd.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionce) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/ce.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructioncf) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/cf.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiond0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/d0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiond1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/d1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiond2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/d2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiond4) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/d4.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiond5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/d5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiond6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/d6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiond7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/d7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiond8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/d8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiond9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/d9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionda) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/da.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiondc) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/dc.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionde) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/de.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructiondf) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/df.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructione0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/e0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructione1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/e1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructione2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/e2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructione5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/e5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructione6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/e6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructione7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/e7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructione8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/e8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructione9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/e9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionea) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/ea.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionee) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/ee.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionef) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/ef.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionf0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/f0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionf1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/f1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionf2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/f2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionf3) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/f3.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionf5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/f5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionf6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/f6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionf7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/f7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionf8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/f8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionf9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/f9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionfa) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/fa.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionfb) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/fb.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, Instructionfe) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/fe.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB00) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB00.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB01) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB01.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB02) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB02.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB03) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB03.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB04) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB04.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB05) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB05.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB06) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB06.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB07) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB07.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB08) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB08.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB09) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB09.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB0a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB0a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB0b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB0b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB0c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB0c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB0d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB0d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB0e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB0e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB0f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB0f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB10) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB10.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB11) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB11.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB12) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB12.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB13) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB13.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB14) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB14.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB15) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB15.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB16) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB16.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB17) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB17.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB18) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB18.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB19) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB19.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB1a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB1a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB1b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB1b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB1c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB1c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB1d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB1d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB1e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB1e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB1f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB1f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB20) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB20.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB21) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB21.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB22) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB22.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB23) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB23.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB24) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB24.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB25) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB25.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB26) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB26.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB27) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB27.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB28) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB28.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB29) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB29.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB2a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB2a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB2b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB2b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB2c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB2c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB2d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB2d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB2e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB2e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB2f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB2f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB30) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB30.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB31) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB31.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB32) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB32.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB33) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB33.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB34) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB34.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB35) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB35.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB36) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB36.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB37) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB37.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB38) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB38.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB39) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB39.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB3a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB3a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB3b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB3b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB3c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB3c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB3d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB3d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB3e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB3e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB3f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB3f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB40) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB40.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB41) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB41.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB42) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB42.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB43) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB43.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB44) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB44.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB45) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB45.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB46) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB46.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB47) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB47.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB48) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB48.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB49) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB49.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB4a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB4a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB4b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB4b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB4c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB4c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB4d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB4d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB4e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB4e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB4f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB4f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB50) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB50.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB51) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB51.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB52) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB52.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB53) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB53.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB54) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB54.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB55) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB55.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB56) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB56.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB57) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB57.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB58) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB58.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB59) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB59.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB5a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB5a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB5b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB5b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB5c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB5c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB5d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB5d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB5e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB5e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB5f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB5f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB60) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB60.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB61) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB61.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB62) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB62.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB63) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB63.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB64) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB64.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB65) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB65.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB66) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB66.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB67) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB67.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB68) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB68.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB69) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB69.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB6a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB6a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB6b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB6b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB6c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB6c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB6d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB6d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB6e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB6e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB6f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB6f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB70) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB70.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB71) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB71.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB72) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB72.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB73) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB73.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB74) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB74.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB75) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB75.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB76) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB76.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB77) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB77.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB78) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB78.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB79) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB79.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB7a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB7a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB7b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB7b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB7c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB7c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB7d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB7d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB7e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB7e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB7f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB7f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB80) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB80.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB81) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB81.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB82) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB82.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB83) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB83.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB84) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB84.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB85) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB85.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB86) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB86.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB87) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB87.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB88) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB88.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB89) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB89.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB8a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB8a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB8b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB8b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB8c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB8c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB8d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB8d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB8e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB8e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB8f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB8f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB90) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB90.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB91) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB91.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB92) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB92.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB93) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB93.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB94) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB94.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB95) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB95.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB96) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB96.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB97) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB97.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB98) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB98.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB99) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB99.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB9a) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB9a.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB9b) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB9b.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB9c) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB9c.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB9d) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB9d.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB9e) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB9e.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCB9f) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CB9f.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBa0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBa0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBa1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBa1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBa2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBa2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBa3) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBa3.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBa4) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBa4.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBa5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBa5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBa6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBa6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBa7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBa7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBa8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBa8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBa9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBa9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBaa) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBaa.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBab) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBab.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBac) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBac.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBad) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBad.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBae) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBae.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBaf) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBaf.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBb0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBb0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBb1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBb1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBb2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBb2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBb3) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBb3.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBb4) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBb4.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBb5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBb5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBb6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBb6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBb7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBb7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBb8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBb8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBb9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBb9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBba) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBba.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBbb) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBbb.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBbc) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBbc.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBbd) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBbd.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBbe) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBbe.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBbf) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBbf.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBc0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBc0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBc1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBc1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBc2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBc2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBc3) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBc3.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBc4) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBc4.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBc5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBc5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBc6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBc6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBc7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBc7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBc8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBc8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBc9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBc9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBca) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBca.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBcb) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBcb.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBcc) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBcc.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBcd) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBcd.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBce) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBce.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBcf) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBcf.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBd0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBd0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBd1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBd1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBd2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBd2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBd3) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBd3.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBd4) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBd4.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBd5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBd5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBd6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBd6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBd7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBd7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBd8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBd8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBd9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBd9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBda) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBda.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBdb) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBdb.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBdc) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBdc.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBdd) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBdd.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBde) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBde.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBdf) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBdf.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBe0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBe0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBe1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBe1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBe2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBe2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBe3) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBe3.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBe4) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBe4.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBe5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBe5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBe6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBe6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBe7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBe7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBe8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBe8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBe9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBe9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBea) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBea.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBeb) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBeb.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBec) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBec.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBed) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBed.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBee) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBee.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBef) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBef.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBf0) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBf0.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBf1) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBf1.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBf2) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBf2.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBf3) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBf3.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBf4) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBf4.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBf5) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBf5.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBf6) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBf6.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBf7) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBf7.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBf8) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBf8.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBf9) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBf9.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBfa) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBfa.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBfb) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBfb.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBfc) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBfc.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBfd) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBfd.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBfe) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBfe.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}

TEST(GameBoyTest, InstructionCBff) {
    json data = loadTestData("/home/ironlanderl/GBEmu/test_data/CBff.json");
    GameBoy gb;

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
            gb.run_tick();
            isItOk = checkFinalState(gb, test);
        }
        
        //gb.advanceStep();
        //isItOk = checkFinalState(gb, test);

        EXPECT_EQ(true, isItOk);

		if (count > MAX){
			break;
		}
		count++;
    }
}
