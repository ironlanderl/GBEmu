#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <nfd.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

#include "GameBoy.h"

class TileViewerWindow
{
public:
    // Color information
    inline static std::map<uint8_t, sf::Color> color_map = {
        { 0b00, sf::Color::White },
        { 0b01, sf::Color(84,84,84)}, // Dark Gray
        { 0b10, sf::Color(169,169,169)}, // Light Gray
        { 0b11, sf::Color::Black }
    };
    static void print_method_1(GameBoy gb, int pixel_size, sf::Vector2f pixel_vec, sf::RenderWindow window);
    static void openWindow(GameBoy gb);
private:
    static constexpr int tile_size = 15;
    static constexpr int num_tiles = 384;

};

