#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
//#include <nfd.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

#include "GameBoy.h"
#include "TileViewerWindow.h"

void openWindow();
void closeWindow();
void loadFile();
void loadFile(std::string outPath);
void drawMemoryViewer();
void drawCartInfo();
void drawCPUInfo();

// Window status
inline bool showMemoryViewer = true;
inline bool showCartInfo = true;
inline bool showCPUInfo = true;