#include "TileViewerWindow.h"
#include <fmt/core.h>

void TileViewerWindow::openWindow(GameBoy gb)
{
    const int pixel_size = 16;
    const sf::Vector2f pixel_vec(pixel_size, pixel_size);

    // DEBUG, REMOVE LATER
    gb.VRAM[0] = 0xFF;
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
    gb.VRAM[31] = 0x00;


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

        window.clear(sf::Color(20,20,20));

        uint8_t row_number = 0;
        uint8_t column_number = 0;
        uint8_t y_index = 0;
        constexpr uint8_t columns_per_row = 8;

        // Loop all VRAM
        for (int i = 0x8000; i < 0x9FFF + 1; i += 2)
        {
            uint8_t bit_1 = gb.getBus(i);
            uint8_t bit_2 = gb.getBus(i + 1);

            for (int j = 0; j < 8; ++j) // Una sola riga di pixel
            {
                uint8_t mask = 0b10000000 >> j;
                uint8_t color = (bit_1 & mask ? 1 : 0) | (bit_2 & mask ? 2 : 0);

                sf::RectangleShape pixel(pixel_vec);
                pixel.setFillColor(color_map[color]);

                // Calculate position based on tile_number, i, and j
                float x_pos = (column_number * pixel_size * 8) + (j * pixel_size);
                float y_pos = (row_number * pixel_size * 8) + (y_index * pixel_size);


                pixel.setPosition(x_pos, y_pos);

                // Use fmt to print debug information
                // fmt::print("Drawing pixel at ({}, {}) with color {}\n", x_pos, y_pos, color);

                window.draw(pixel);
            }

            y_index++;

            if (y_index > 7)
            {
                column_number++;
                y_index = 0;
            }

            if (column_number > columns_per_row)
            {
                column_number = 0;
                row_number++;
                y_index = 0;
            }

        }

        window.display();
    }
}
