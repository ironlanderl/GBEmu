#include "Window.h"

// global GB
GameBoy gb;

void openWindow()
{
	bool single_run = false;

	sf::RenderWindow window(sf::VideoMode(1280, 720), "ImGui + SFML = <3");
	window.setFramerateLimit(60);
	if (!ImGui::SFML::Init(window))
	{
		std::cerr << "Failed to initialize ImGui with SFML" << std::endl;
		return;
	}

	sf::Clock deltaClock;
	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
			{
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
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
				// loadFile();
			}
			if (ImGui::MenuItem("Open Dr. Mario"))
			{
				loadFile("/home/ironlanderl/mario.gb");
			}
			if (ImGui::MenuItem("Open Tetris"))
			{
				loadFile("/home/ironlanderl/tetris.gb");
			}
			if (ImGui::MenuItem("Open cpu_instrs.gb"))
			{
				loadFile("/home/ironlanderl/cpu_instrs/cpu_instrs.gb");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::MenuItem("Open Cart Information"))
			{
				showCartInfo = !showCartInfo;
			}
			if (ImGui::MenuItem("Open Memory Viewer"))
			{
				showMemoryViewer = !showMemoryViewer;
			}
			if (ImGui::MenuItem("Open CPU Info"))
			{
				showCPUInfo = !showCPUInfo;
			}
			if (ImGui::MenuItem("Open Tile Viewer"))
			{
				TileViewerWindow::openWindow(gb);
			}
			if (ImGui::MenuItem("Open Audio Viewer"))
			{
			std:
				printf("Open audio viewer window\n");
			}
			if (ImGui::MenuItem("Open OAM Viewer"))
			{
				showOAM = !showOAM;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		// ImGui::ShowDemoWindow();
		drawMemoryViewer();
		drawCartInfo();
		drawCPUInfo();
		drawOAM();

		// Run emulator
		if (gb.status == RUNNING)
		{
			// TODO: POTER CAMBIARE IL NUMERO DI CICLI EFFETTUATI
			for (int i = 0; i < 1000; i++)
			{
				gb.execute_step();
				// Debugging: Print serial to console
				if (gb.getBus(0xFF02) == 0b10000001)
				{
					char asciiChar = static_cast<char>(gb.getBus(0xFF01));
					printf("%c", asciiChar);
					// Reset SC
					gb.writeBus(0x01, 0xFF02);
				}
			}
			// gb.status = PAUSED;
		}

		window.clear();

		// Render Tile Map
		for (int x = 0; x < 144; x++)
		{
			for (int y = 0; y < 160; y++)
			{
				uint8_t tile_index = gb.getBus(0x9800 + x + y * 160);
				sf::Color pixel_color = getPixelColor(getTileData(gb, tile_index), x % 8, y % 8);
				sf::RectangleShape pixel(sf::Vector2f(1, 1));
				pixel.setFillColor(pixel_color);
				pixel.setPosition(x, y);
				window.draw(pixel);
			}
			gb.writeBus(x, 0xFF44);
		}

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
	if (showMemoryViewer)
	{
		ImGui::Begin("Memory Viewer");

		// Display memory content here
		for (int i = 0; i < 0x1000; i += 16)
		{
			ImGui::Text("%04X:", i);

			// Display hexadecimal values
			for (int j = 0; j < 16; ++j)
			{
				ImGui::SameLine();
				// Check if the current memory cell is the same as the program counter (PC)
				if ((i + j) == gb.PC)
				{
					// Change selected memory cell to green.
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), " %02X", gb.ROM[i + j]);
				}
				else
				{
					// Display normal hexadecimal value
					ImGui::Text(" %02X", gb.ROM[i + j]);
				}
				ImGui::SameLine();
				if (j == 7)
					ImGui::Dummy(ImVec2(5.0f, 0.0f)); // Add space between the two halves
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

			// ImGui::NewLine();
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

		ImGui::Text("Mapper: %d", gb.getBus(0x0147));

		ImGui::End();
	}
}

void drawOAM()
{
	if (showOAM)
	{
		ImGui::Begin("OAM Info");

		for (int i = 0x0; i < 0x9F; i += 4)
		{
			ImGui::Text("Sprite %d", i / 4);
			ImGui::SameLine();
			ImGui::Text("Y: %d", gb.getBus(0xFE00 + i));
			ImGui::SameLine();
			ImGui::Text("X: %d", gb.getBus(0xFE00 + i + 1));
			ImGui::SameLine();
			ImGui::Text("Tile: %d", gb.getBus(0xFE00 + i + 2));
			ImGui::SameLine();
			ImGui::Text("Flags: %d", gb.getBus(0xFE00 + i + 3));
		}

		ImGui::Text("Mapper: %d", gb.getBus(0x0147));

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
				gb.execute_step();
			}
			ImGui::SameLine();
			if (ImGui::Button("Resume"))
			{
				gb.status = RUNNING;
			}
		}

		if (gb.status == RUNNING)
		{
			if (ImGui::Button("Pause"))
			{
				gb.status = PAUSED;
			}
		}

		ImGui::Text("Registers:");
		ImGui::Text("A: %02X   B: %02X   C: %02X   D: %02X", gb.A, gb.B, gb.C, gb.D);
		ImGui::Text("E: %02X   H: %02X   L: %02X   F: ", gb.E, gb.H, gb.L);

		// Printing of F(lags)
		// Zero byte
		ImGui::SameLine();
		ImGui::Text("%d", gb.ZeroFlag);
		// Negative Byte
		ImGui::SameLine();
		ImGui::Text("%d", gb.NegativeFlag);
		// Half Carry
		ImGui::SameLine();
		ImGui::Text("%d", gb.HalfCarry);
		// Carry
		ImGui::SameLine();
		ImGui::Text("%d", gb.Carry);

		ImGui::Text("PC: %04X  SP: %04X", gb.PC, gb.SP);

		// Serial information
		bool trans_enabled = gb.getBus(0xFF02) & 0b10000000;
		bool clock_double = gb.getBus(0xFF02) & 0b00000010;
		bool which_clock = gb.getBus(0xFF02) & 0b00000001;
		ImGui::Text("Transfer enabled: %d, Clock doubled : %0d, Clock: %s", trans_enabled, clock_double, which_clock == 0 ? "Slave" : "Master");

		// Additional CPU information can be added here based on your requirements

		ImGui::End();
	}
}

/*void loadFile()
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
}*/

void loadFile(std::string outPath)
{
	std::printf("Success!\nLoading ROM to RAM\n");

	// Use RAII to manage the file resource
	std::ifstream romFile(outPath, std::ios::binary | std::ios::ate);

	if (romFile.is_open())
	{
		std::streamsize fileSize = romFile.tellg();
		romFile.seekg(0, std::ios::beg);

		// Use std::vector for dynamic memory allocation and automatic cleanup
		std::vector<char> buffer(fileSize);

		if (romFile.read(reinterpret_cast<char *>(buffer.data()), fileSize))
		{
			// Assuming GB is an instance of your GameBoy class
			gb.loadRom(buffer.data(), fileSize);
			std::printf("ROM loaded successfully.\n");
			gb.status = PAUSED;
		}
		else
		{
			std::printf("Error reading ROM file.\n");
		}
	}
	else
	{
		std::printf("Error opening ROM file.\n");
	}
}