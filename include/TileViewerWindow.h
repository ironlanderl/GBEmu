#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <imgui.h>
#include <iostream>
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
        { 0b01, sf::Color(84, 84, 84) }, // Dark Gray
        { 0b10, sf::Color(169, 169, 169) }, // Light Gray
        { 0b11, sf::Color::Black }
    };

    static void openWindow(GameBoy gb);

private:
    static constexpr int tile_size = 16;
    static constexpr int num_tiles = 384;
    static constexpr uint8_t columns_per_row = 8;

    static void drawTile(sf::RenderWindow& window, GameBoy& gb, int tile_index, int x_offset, int y_offset);
    static void drawAllTiles(sf::RenderWindow& window, GameBoy& gb);
};
