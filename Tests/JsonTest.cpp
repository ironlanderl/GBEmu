#include "pch.h"
#include "JsonTest.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

void JsonTest::runtest()
{
	using json = nlohmann::json;

	// Carica i dati dall'esempio fornito
	json data;
	std::ifstream file("data.json");
	if (file.is_open()) {
		file >> data;
		file.close();
	}
	else {
		std::cerr << "Errore durante l'apertura del file." << std::endl;
	}

	// Estrai i dati dall'oggetto JSON
	std::string reg_a = data["initial"]["a"];
	std::string reg_b = data["initial"]["b"];
	std::string reg_c = data["initial"]["c"];
	std::string reg_d = data["initial"]["d"];
	std::string reg_e = data["initial"]["e"];
	std::string reg_f = data["initial"]["f"];
	std::string reg_h = data["initial"]["h"];
	std::string reg_l = data["initial"]["l"];
	std::string reg_pc = data["initial"]["pc"];
	std::string reg_sp = data["initial"]["sp"];
	std::vector<std::pair<std::string, std::string>> ram_initial;
	for (const auto& entry : data["initial"]["ram"]) {
		ram_initial.push_back({ entry[0], entry[1] });
	}
	std::vector<std::pair<std::string, std::string>> ram_final;
	for (const auto& entry : data["final"]["ram"]) {
		ram_final.push_back({ entry[0], entry[1] });
	}
	std::vector<std::tuple<std::string, std::string, std::string>> cycles;
	for (const auto& cycle : data["cycles"]) {
		cycles.push_back({ cycle[0], cycle[1], cycle[2] });
	}

	// Stampa gli stati iniziali e finali
	std::cout << "Initial State:" << std::endl;
	std::cout << "A: " << reg_a << std::endl;
	std::cout << "B: " << reg_b << std::endl;
	std::cout << "C: " << reg_c << std::endl;
	std::cout << "D: " << reg_d << std::endl;
	std::cout << "E: " << reg_e << std::endl;
	std::cout << "F: " << reg_f << std::endl;
	std::cout << "H: " << reg_h << std::endl;
	std::cout << "L: " << reg_l << std::endl;
	std::cout << "PC: " << reg_pc << std::endl;
	std::cout << "SP: " << reg_sp << std::endl;
	std::cout << "RAM Initial:" << std::endl;
	for (const auto& entry : ram_initial) {
		std::cout << "[" << entry.first << ", " << entry.second << "]" << std::endl;
	}

	std::cout << std::endl;

	std::cout << "Final State:" << std::endl;
	std::cout << "A: " << reg_a << std::endl;
	std::cout << "B: " << reg_b << std::endl;
	std::cout << "C: " << reg_c << std::endl;
	std::cout << "D: " << reg_d << std::endl;
	std::cout << "E: " << reg_e << std::endl;
	std::cout << "F: " << reg_f << std::endl;
	std::cout << "H: " << reg_h << std::endl;
	std::cout << "L: " << reg_l << std::endl;
	std::cout << "PC: " << reg_pc << std::endl;
	std::cout << "SP: " << reg_sp << std::endl;
	std::cout << "RAM Final:" << std::endl;
	for (const auto& entry : ram_final) {
		std::cout << "[" << entry.first << ", " << entry.second << "]" << std::endl;
	}

	// Stampa i cicli di esecuzione
	std::cout << std::endl << "Execution Cycles:" << std::endl;
	for (const auto& cycle : cycles) {
		std::cout << "Address: " << std::get<0>(cycle) << ", Data: " << std::get<1>(cycle) << ", Operation: " << std::get<2>(cycle) << std::endl;
	}

}
