
#include "GameBoy.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <gtest/gtest.h>
#include <cstdlib> // For std::strtoul

using json = nlohmann::json;

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
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\00.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction02) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\02.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction07) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\07.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction08) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\08.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction09) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\09.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction0A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\0A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction0F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\0F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction12) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\12.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction17) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\17.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction19) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\19.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction1A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\1A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction1F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\1F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction22) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\22.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction27) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\27.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction29) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\29.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction2A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\2A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction2C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\2C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction36) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\36.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction37) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\37.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction39) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\39.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction3A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\3A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction3F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\3F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction46) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\46.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction56) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\56.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction66) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\66.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction70) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\70.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction71) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\71.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction72) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\72.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction73) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\73.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction74) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\74.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction75) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\75.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction76) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\76.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction77) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\77.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction86) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\86.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction96) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\96.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction98) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\98.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction99) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\99.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction9A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\9A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction9B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\9B.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction9C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\9C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction9D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\9D.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction9E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\9E.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, Instruction9F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\9F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionA8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\A8.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionA9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\A9.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionAA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\AA.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionAB) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\AB.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionAC) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\AC.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionAD) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\AD.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionAE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\AE.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionB0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\B0.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionB1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\B1.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionB2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\B2.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionB3) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\B3.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionB4) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\B4.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionB5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\B5.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionB6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\B6.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionB7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\B7.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionC4) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\C4.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCC) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CC.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionD4) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\D4.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionDC) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\DC.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionDE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\DE.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionE2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\E2.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionE8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\E8.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionEE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\EE.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionF2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\F2.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionF6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\F6.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionF8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\F8.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB00) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB00.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB01) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB01.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB02) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB02.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB03) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB03.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB04) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB04.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB05) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB05.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB06) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB06.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB07) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB07.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB08) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB08.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB09) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB09.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB0A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB0A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB0B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB0B.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB0C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB0C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB0D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB0D.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB0E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB0E.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB0F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB0F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB10) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB10.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB11) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB11.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB12) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB12.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB13) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB13.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB14) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB14.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB15) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB15.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB16) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB16.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB17) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB17.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB18) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB18.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB19) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB19.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB1A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB1A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB1B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB1B.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB1C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB1C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB1D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB1D.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB1E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB1E.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB1F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB1F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB20) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB20.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB21) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB21.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB22) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB22.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB23) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB23.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB24) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB24.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB25) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB25.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB26) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB26.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB27) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB27.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB28) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB28.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB29) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB29.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB2A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB2A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB2B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB2B.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB2C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB2C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB2D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB2D.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB2E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB2E.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB2F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB2F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB30) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB30.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB31) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB31.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB32) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB32.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB33) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB33.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB34) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB34.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB35) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB35.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB36) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB36.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB37) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB37.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB38) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB38.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB39) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB39.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB3A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB3A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB3B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB3B.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB3C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB3C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB3D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB3D.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB3E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB3E.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB3F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB3F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB40) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB40.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB41) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB41.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB42) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB42.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB43) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB43.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB44) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB44.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB45) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB45.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB46) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB46.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB47) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB47.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB48) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB48.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB49) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB49.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB4A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB4A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB4B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB4B.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB4C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB4C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB4D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB4D.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB4E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB4E.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB4F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB4F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB50) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB50.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB51) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB51.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB52) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB52.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB53) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB53.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB54) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB54.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB55) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB55.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB56) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB56.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB57) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB57.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB58) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB58.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB59) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB59.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB5A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB5A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB5B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB5B.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB5C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB5C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB5D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB5D.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB5E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB5E.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB5F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB5F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB60) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB60.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB61) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB61.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB62) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB62.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB63) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB63.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB64) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB64.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB65) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB65.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB66) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB66.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB67) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB67.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB68) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB68.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB69) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB69.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB6A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB6A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB6B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB6B.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB6C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB6C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB6D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB6D.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB6E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB6E.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB6F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB6F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB70) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB70.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB71) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB71.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB72) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB72.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB73) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB73.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB74) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB74.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB75) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB75.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB76) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB76.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB77) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB77.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB78) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB78.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB79) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB79.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB7A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB7A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB7B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB7B.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB7C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB7C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB7D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB7D.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB7E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB7E.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB7F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB7F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB80) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB80.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB81) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB81.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB82) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB82.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB83) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB83.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB84) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB84.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB85) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB85.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB86) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB86.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB87) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB87.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB88) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB88.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB89) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB89.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB8A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB8A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB8B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB8B.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB8C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB8C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB8D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB8D.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB8E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB8E.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB8F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB8F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB90) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB90.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB91) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB91.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB92) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB92.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB93) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB93.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB94) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB94.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB95) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB95.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB96) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB96.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB97) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB97.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB98) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB98.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB99) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB99.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB9A) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB9A.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB9B) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB9B.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB9C) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB9C.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB9D) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB9D.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB9E) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB9E.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCB9F) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CB9F.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBA0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBA0.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBA1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBA1.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBA2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBA2.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBA3) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBA3.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBA4) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBA4.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBA5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBA5.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBA6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBA6.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBA7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBA7.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBA8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBA8.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBA9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBA9.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBAA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBAA.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBAB) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBAB.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBAC) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBAC.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBAD) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBAD.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBAE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBAE.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBAF) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBAF.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBB0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBB0.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBB1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBB1.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBB2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBB2.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBB3) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBB3.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBB4) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBB4.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBB5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBB5.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBB6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBB6.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBB7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBB7.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBB8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBB8.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBB9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBB9.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBBA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBBA.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBBB) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBBB.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBBC) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBBC.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBBD) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBBD.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBBE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBBE.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBBF) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBBF.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBC0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBC0.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBC1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBC1.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBC2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBC2.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBC3) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBC3.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBC4) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBC4.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBC5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBC5.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBC6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBC6.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBC7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBC7.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBC8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBC8.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBC9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBC9.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBCA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBCA.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBCB) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBCB.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBCC) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBCC.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBCD) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBCD.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBCE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBCE.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBCF) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBCF.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBD0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBD0.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBD1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBD1.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBD2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBD2.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBD3) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBD3.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBD4) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBD4.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBD5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBD5.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBD6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBD6.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBD7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBD7.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBD8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBD8.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBD9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBD9.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBDA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBDA.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBDB) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBDB.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBDC) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBDC.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBDD) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBDD.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBDE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBDE.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBDF) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBDF.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBE0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBE0.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBE1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBE1.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBE2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBE2.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBE3) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBE3.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBE4) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBE4.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBE5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBE5.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBE6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBE6.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBE7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBE7.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBE8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBE8.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBE9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBE9.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBEA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBEA.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBEB) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBEB.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBEC) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBEC.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBED) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBED.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBEE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBEE.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBEF) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBEF.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBF0) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBF0.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBF1) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBF1.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBF2) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBF2.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBF3) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBF3.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBF4) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBF4.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBF5) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBF5.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBF6) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBF6.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBF7) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBF7.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBF8) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBF8.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBF9) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBF9.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBFA) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBFA.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBFB) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBFB.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBFC) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBFC.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBFD) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBFD.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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

TEST(GameBoyTest, InstructionCBFE) {
    json data = loadTestData("C:\\Users\\fabri\\source\\GBEmu\\test_data\\CBFE.json");
    GameBoy gb;

	const int max = 10;
	int count = 0;

    for (const auto& test : data) {
        // debug
        std::printf("Running Test: %s\n", test["name"].get<std::string>().c_str());
        bool isItOk;

        gb.loadRom({}, 0);
        // Load test data
        loadTestDataToGameBoy(gb, test);

        // Run instruction if allowed
        if (shouldSkip) {
            isItOk = true;
            shouldSkip = false;
        }
        else {
            gb.run_tick();
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
