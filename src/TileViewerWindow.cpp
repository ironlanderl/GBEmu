#include "TileViewerWindow.h"
#include <fmt/core.h>

const int pixel_size = 16;
const sf::Vector2f pixel_vec(pixel_size, pixel_size);
constexpr uint8_t columns_per_row = 8;

void TileViewerWindow::drawTile(sf::RenderWindow& window, GameBoy& gb, int tile_index, int x_offset, int y_offset)
{
    uint8_t y_index = 0;
    for (int i = 0; i < 16; i += 2)
    {
        uint8_t bit_1 = gb.getBus(0x8000 + tile_index * 16 + i);
        uint8_t bit_2 = gb.getBus(0x8000 + tile_index * 16 + i + 1);

        for (int j = 0; j < 8; ++j)
        {
            uint8_t mask = 0b10000000 >> j;
            uint8_t color = (bit_1 & mask ? 1 : 0) | (bit_2 & mask ? 2 : 0);

            sf::RectangleShape pixel(pixel_vec);
            pixel.setFillColor(color_map[color]);

            float x_pos = x_offset + (j * pixel_size);
            float y_pos = y_offset + (y_index * pixel_size);

            pixel.setPosition(x_pos, y_pos);
            window.draw(pixel);
        }
        y_index++;
    }
}

void TileViewerWindow::drawAllTiles(sf::RenderWindow& window, GameBoy& gb)
{
    uint8_t row_number = 0;
    uint8_t column_number = 0;

    for (int tile_index = 0; tile_index < 384; ++tile_index)
    {
        int x_offset = column_number * pixel_size * 8;
        int y_offset = row_number * pixel_size * 8;

        drawTile(window, gb, tile_index, x_offset, y_offset);

        column_number++;
        if (column_number >= columns_per_row)
        {
            column_number = 0;
            row_number++;
        }
    }
}

void TileViewerWindow::openWindow(GameBoy gb)
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Tile Viewer");
    const int pixel_size = 16;
    const sf::Vector2f pixel_vec(pixel_size, pixel_size);

    // DEBUG, REMOVE LATER
    /*gb.VRAM[0] = 0xFF;
    gb.VRAM[1] = 0x00;
    gb.VRAM[2] = 0x7E;
    gb.VRAM[3] = 0xFF;
    gb.VRAM[4] = 0x85;
    gb.VRAM[5] = 0x81;
    gb.VRAM[6] = 0x89;
    gb.VRAM[7] = 0x83;
    gb.VRAM[8] = 0x93;
    gb.VRAM[9] = 0x85;
    gb.VRAM[10] = 0xA5;
    gb.VRAM[11] = 0x8B;
    gb.VRAM[12] = 0xC9;
    gb.VRAM[13] = 0x97;
    gb.VRAM[14] = 0x7E;
    gb.VRAM[15] = 0xFF;

    gb.VRAM[16] = 0x7C;
    gb.VRAM[17] = 0x7C;
    gb.VRAM[18] = 0x00;
    gb.VRAM[19] = 0xC6;
    gb.VRAM[20] = 0xC6;
    gb.VRAM[21] = 0x00;
    gb.VRAM[22] = 0x00;
    gb.VRAM[23] = 0xFE;
    gb.VRAM[24] = 0xC6;
    gb.VRAM[25] = 0xC6;
    gb.VRAM[26] = 0x00;
    gb.VRAM[27] = 0xC6;
    gb.VRAM[28] = 0xC6;
    gb.VRAM[29] = 0x00;
    gb.VRAM[30] = 0x00;
    gb.VRAM[31] = 0x00;*/


    sf::RenderWindow window(sf::VideoMode({800, 600}), "Tile Viewer");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color(20, 20, 20));
        drawAllTiles(window, gb);
        window.display();
    }
}
