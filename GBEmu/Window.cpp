#include "Window.h"

// global GB
GameBoy gb;

void openWindow()
{
	sf::RenderWindow window(sf::VideoMode(1280, 720), "ImGui + SFML = <3");
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	sf::Clock deltaClock;
	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		// add toolbar with the following options:
		/*
		 * - File
		 * -- Open
		 * -- Open Recent
		 *
		 * - Debug
		 * -- Open Debugger
		 * -- Open Disassembler
		 * -- Open Memory Viewer
		 * -- Open Tile Viewer
		 * -- Open Audio Viewer
		 */
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open")) {
				loadFile();
			}
			if (ImGui::MenuItem("Open Tetris")) {
				loadFile("C:\\Users\\fabri\\Downloads\\Tetris (World) (Rev A).gb");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug")) {
			if (ImGui::MenuItem("Open Cart Information")) {
				showCartInfo = !showCartInfo;
			}
			if (ImGui::MenuItem("Open Memory Viewer")) {
				showMemoryViewer = !showMemoryViewer;
			}
			if (ImGui::MenuItem("Open CPU Info")) {
				showCPUInfo = !showCPUInfo;
			}
			if (ImGui::MenuItem("Open Tile Viewer")) {
				fmt::print("Open tile viewer window\n");
			}
			if (ImGui::MenuItem("Open Audio Viewer")) {
				fmt::print("Open audio viewer window\n");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		// ImGui::ShowDemoWindow();
		drawMemoryViewer();
		drawCartInfo();
		drawCPUInfo();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}
}

void closeWindow()
{
	ImGui::SFML::Shutdown();
}

void drawMemoryViewer()
{
	if (showMemoryViewer) {
		ImGui::Begin("Memory Viewer");

		// Display memory content here
		for (int i = 0; i < 0x1000; i += 16)
		{
			ImGui::Text("%04X:", i);

			// Display hexadecimal values
			for (int j = 0; j < 16; ++j)
			{
				ImGui::SameLine();
				ImGui::Text(" %02X", gb.ROM[i + j]);
				ImGui::SameLine();
				if (j == 7) ImGui::Dummy(ImVec2(5.0f, 0.0f)); // Add space between the two halves
			}

			// Display ASCII values
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(5.0f, 0.0f));

			for (int j = 0; j < 16; ++j)
			{
				ImGui::SameLine();
				char asciiChar = (gb.ROM[i + j] >= 32 && gb.ROM[i + j] <= 126) ? static_cast<char>(gb.ROM[i + j]) : '.';
				ImGui::Text("%c", asciiChar);
			}

			//ImGui::NewLine();
		}

		ImGui::End();
	}
}

void drawCartInfo()
{
	if (showCartInfo)
	{
		ImGui::Begin("Cart Info");

		ImGui::Text("Title: ");
		for (int i = 0x0134; i < 0x0144; i++)
		{
			ImGui::SameLine();
			ImGui::Text("%c", static_cast<char>(gb.ROM[i]));
		}
		ImGui::End();
	}
}

void drawCPUInfo()
{
	if (showCPUInfo)
	{
		ImGui::Begin("CPU Information");

		if (gb.status == PAUSED)
		{
			if (ImGui::Button("Next Step"))
			{
				gb.advanceStep();
			}
			ImGui::SameLine();
			if (ImGui::Button("Resume"))
			{
				fmt::print("Resume\n");
			}
		}

		ImGui::Text("Registers:");
		ImGui::Text("A: %02X   B: %02X   C: %02X   D: %02X", gb.A, gb.B, gb.C, gb.D);
		ImGui::Text("E: %02X   H: %02X   L: %02X   F: ", gb.E, gb.H, gb.L);


		// Printing of F(lags)
		// Zero byte
		ImGui::SameLine();
		ImGui::Text("%d", (gb.F & 0b10000000) >> 7);
		// Negative Byte
		ImGui::SameLine();
		ImGui::Text("%d", (gb.F & 0b01000000) >> 6);
		// Half Carry
		ImGui::SameLine();
		ImGui::Text("%d", (gb.F & 0b00100000) >> 5);
		// Carry
		ImGui::SameLine();
		ImGui::Text("%d", (gb.F & 0b00010000) >> 4);




		ImGui::Text("PC: %04X  SP: %04X", gb.PC, gb.SP);

		// Additional CPU information can be added here based on your requirements

		ImGui::End();
	}
}


void loadFile()
{
	nfdchar_t* outPath = nullptr;
	nfdchar_t filter[] = "gb";
	nfdresult_t result = NFD_OpenDialog(filter, nullptr, &outPath);

	if (result == NFD_OKAY) {
		fmt::println("Success!\nLoading ROM to RAM");

		// Use RAII to manage the file resource
		std::ifstream romFile(outPath, std::ios::binary | std::ios::ate);

		if (romFile.is_open()) {
			std::streamsize fileSize = romFile.tellg();
			romFile.seekg(0, std::ios::beg);

			// Use std::vector for dynamic memory allocation and automatic cleanup
			std::vector<char> buffer(fileSize);

			if (romFile.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
				// Assuming GB is an instance of your GameBoy class
				gb.loadRom(buffer.data(), fileSize);
				fmt::println("ROM loaded successfully.");
				gb.status = PAUSED;
			}
			else {
				fmt::println("Error reading ROM file.");
			}
		}
		else {
			fmt::println("Error opening ROM file.");
		}
	}
	else if (result == NFD_CANCEL) {
		fmt::println("User pressed cancel.");
	}
	else {
		fmt::println("Error: {}\n", NFD_GetError());
	}

	// NFD automatically allocates memory for outPath, so there's no need to free it
}

void loadFile(std::string outPath)
{
	fmt::println("Success!\nLoading ROM to RAM");

	// Use RAII to manage the file resource
	std::ifstream romFile(outPath, std::ios::binary | std::ios::ate);

	if (romFile.is_open()) {
		std::streamsize fileSize = romFile.tellg();
		romFile.seekg(0, std::ios::beg);

		// Use std::vector for dynamic memory allocation and automatic cleanup
		std::vector<char> buffer(fileSize);

		if (romFile.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
			// Assuming GB is an instance of your GameBoy class
			gb.loadRom(buffer.data(), fileSize);
			fmt::println("ROM loaded successfully.");
			gb.status = PAUSED;
		}
		else {
			fmt::println("Error reading ROM file.");
		}
	}
	else {
		fmt::println("Error opening ROM file.");
	}

	// NFD automatically allocates memory for outPath, so there's no need to free it
}