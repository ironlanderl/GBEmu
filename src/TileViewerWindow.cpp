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
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color(20, 20, 20));
        drawAllTiles(window, gb);
        window.display();
    }
}
