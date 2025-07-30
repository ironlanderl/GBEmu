#include "Tiles.h"

/**
 * @brief Retrieves the tile data for a given tile index from the GameBoy's memory.
 *
 * This function extracts 16 bytes of tile data starting from the specified tile index.
 * The tile data is fetched from the GameBoy's video memory starting at address 0x8000.
 *
 * @param gb Reference to the GameBoy instance from which to retrieve the tile data.
 * @param tile_index The index of the tile to retrieve. Each tile consists of 16 bytes.
 * @return A std::array containing the 16 bytes of tile data.
 */
std::array<uint8_t, 16> getTileData(GameBoy& gb, int tile_index)
{
    std::array<uint8_t, 16> tile_data;
    for (int i = 0; i < 16; ++i)
    {
        tile_data[i] = gb.getBus(0x8000 + tile_index * 16 + i);
    }
    return tile_data;
}

/**
 * @brief Draws a tile on the given SFML render window.
 *
 * This function takes tile data in the form of a 16-byte array and draws an 8x8 tile
 * on the specified SFML render window. Each byte in the tile data represents two bits
 * of color information for each pixel in the tile.
 *
 * @param renderer The SFML render window where the tile will be drawn.
 * @param tile_data A 16-byte array containing the tile data. Each pair of bytes represents
 *                  the color information for one row of the tile.
 * @param x_offset The x-coordinate offset where the tile will be drawn.
 * @param y_offset The y-coordinate offset where the tile will be drawn.
 * @param pixel_size The size of each pixel in the tile.
 *
 * The color mapping is as follows:
 * - 0: White (255, 255, 255)
 * - 1: Light gray (192, 192, 192)
 * - 2: Dark gray (96, 96, 96)
 * - 3: Black (0, 0, 0)
 */
void drawTile(sf::RenderWindow &renderer, const std::array<uint8_t, 16>& tile_data, int x_offset, int y_offset, int pixel_size)
{
    // Loop through each row of the tile (8 rows in total)
    for (int y = 0; y < 8; ++y)
    {
        // Each row is represented by two bytes in the tile data
        uint8_t bit_1 = tile_data[y * 2];
        uint8_t bit_2 = tile_data[y * 2 + 1];

        // Loop through each column of the tile (8 columns in total)
        for (int x = 0; x < 8; ++x)
        {
            // Create a mask to isolate the bit for the current pixel
            uint8_t mask = 0b10000000 >> x;
            // Determine the color of the pixel by combining the bits from the two bytes
            uint8_t color = (bit_1 & mask ? 1 : 0) | (bit_2 & mask ? 2 : 0);

            // Create a rectangle shape to represent the pixel
            sf::RectangleShape pixel_rect(sf::Vector2f(pixel_size, pixel_size));
            // Set the position of the pixel on the render window
            pixel_rect.setPosition(x_offset + x * pixel_size, y_offset + y * pixel_size);

            // Set the color of the pixel based on the color value
            switch (color)
            {
                case 0:
                    pixel_rect.setFillColor(sf::Color(255, 255, 255)); // White
                    break;
                case 1:
                    pixel_rect.setFillColor(sf::Color(192, 192, 192)); // Light gray
                    break;
                case 2:
                    pixel_rect.setFillColor(sf::Color(96, 96, 96)); // Dark gray
                    break;
                case 3:
                    pixel_rect.setFillColor(sf::Color(0, 0, 0)); // Black
                    break;
            }

            // Draw the pixel on the render window
            renderer.draw(pixel_rect);
        }
    }
}

/**
 * @brief Retrieves the color of a single pixel from the tile data.
 *
 * This function takes tile data in the form of a 16-byte array and returns the color
 * of the pixel at the specified (x, y) coordinates within the tile.
 *
 * @param tile_data A 16-byte array containing the tile data. Each pair of bytes represents
 *                  the color information for one row of the tile.
 * @param x The x-coordinate of the pixel within the tile (0-7).
 * @param y The y-coordinate of the pixel within the tile (0-7).
 * @return An sf::Color object representing the color of the pixel.
 *
 * The color mapping is as follows:
 * - 0: White (255, 255, 255)
 * - 1: Light gray (192, 192, 192)
 * - 2: Dark gray (96, 96, 96)
 * - 3: Black (0, 0, 0)
 */
sf::Color getPixelColor(const std::array<uint8_t, 16>& tile_data, int x, int y)
{
    // Each row is represented by two bytes in the tile data
    uint8_t bit_1 = tile_data[y * 2];
    uint8_t bit_2 = tile_data[y * 2 + 1];

    // Create a mask to isolate the bit for the current pixel
    uint8_t mask = 0b10000000 >> x;
    // Determine the color of the pixel by combining the bits from the two bytes
    uint8_t color = (bit_1 & mask ? 1 : 0) | (bit_2 & mask ? 2 : 0);

    // Return the color of the pixel based on the color value
    switch (color)
    {
        case 0:
            return sf::Color(255, 255, 255); // White
        case 1:
            return sf::Color(192, 192, 192); // Light gray
        case 2:
            return sf::Color(96, 96, 96); // Dark gray
        case 3:
            return sf::Color(0, 0, 0); // Black
        default:
            return sf::Color(255, 255, 255); // Default to white if something goes wrong
    }
}