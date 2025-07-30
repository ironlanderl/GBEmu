#pragma once

#include <array>
#include <cstdint>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include "GameBoy.h"

std::array<uint8_t, 16> getTileData(GameBoy& gb, int tile_index);
void drawTile(sf::RenderWindow &renderer, const std::array<uint8_t, 16>& tile_data, int x_offset, int y_offset, int pixel_size);
sf::Color getPixelColor(const std::array<uint8_t, 16>& tile_data, int x, int y);
