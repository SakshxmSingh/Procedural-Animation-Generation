#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For seeding rand()
#include <iostream>

#define GRID_SPACING 5 // Size of each cell (40x40 pixels)
#define WALL_PROBABILITY 0.6 // Probability of a cell being a wall
#define CA_STEPS 5 // Number of Cellular Automata steps

void drunkWalk(std::vector<std::vector<int>>& gridColors, int steps) {
    int rows = gridColors.size();
    int cols = gridColors[0].size();
    
    // Start position (randomly chosen)
    // int row = rand() % rows;
    // int col = rand() % cols;

    // // start at the bottom left corner
    // int row = rows - 1;
    // int col = 0;

    // randomly decide the corner to start at
    int corner = rand() % 4;
    int row, col;
    if (corner == 0) {
        row = rows - 1;
        col = 0;
    } else if (corner == 1) {
        row = 0;
        col = 0;
    } else if (corner == 2) {
        row = 0;
        col = cols - 1;
    } else {
        row = rows - 1;
        col = cols - 1;
    }
    
    for (int i = 0; i < steps; ++i) {
        gridColors[row][col] = 1; // Paint current cell black
        
        // Randomly choose a direction: 0 = up, 1 = down, 2 = left, 3 = right
        int direction = rand() % 4;
        
        // Move in the chosen direction, ensuring we stay within bounds
        if (direction == 0 && row > 0) row--;           // Up
        else if (direction == 1 && row < rows - 1) row++; // Down
        else if (direction == 2 && col > 0) col--;        // Left
        else if (direction == 3 && col < cols - 1) col++; // Right
    }
}
// Function to initialize the grid randomly
void initializeGrid(std::vector<std::vector<int>>& grid, float wallProbability) {
    for (int row = 0; row < grid.size(); ++row) {
        for (int col = 0; col < grid[0].size(); ++col) {
            grid[row][col] = (rand() / static_cast<float>(RAND_MAX)) < wallProbability ? 1 : 0;
        }
    }

    // Set the outer border to be walls
    for (int i = 0; i < grid.size(); ++i) {
        grid[i][0] = 1;
        grid[i][grid[0].size() - 1] = 1;
    }
    for (int i = 0; i < grid[0].size(); ++i) {
        grid[0][i] = 1;
        grid[grid.size() - 1][i] = 1;
    }

    // Set the start and end points
    grid[grid.size() - 1][0] = 0;
    grid[0][grid[0].size() - 1] = 0;
}

// Function to count the number of wall neighbors around a cell
int countWallNeighbors(const std::vector<std::vector<int>>& grid, int row, int col) {
    int wallCount = 0;
    int rows = grid.size();
    int cols = grid[0].size();

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            int newRow = row + i;
            int newCol = col + j;
            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                wallCount += grid[newRow][newCol];
            }
        }
    }
    return wallCount;
}

// Function to apply the Cellular Automata smoothing rules
std::vector<std::vector<int>> applyCARules(std::vector<std::vector<int>> grid) {
    std::vector<std::vector<int>> newGrid = grid;

    int rows = grid.size();
    int cols = grid[0].size();

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int wallNeighbors = countWallNeighbors(grid, row, col);

            if (wallNeighbors >= 5) newGrid[row][col] = 1;
            else newGrid[row][col] = 0;
        }
    }
    return newGrid;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Cellular Automata Maze");

    int rows = window.getSize().y / GRID_SPACING;
    int cols = window.getSize().x / GRID_SPACING;

    std::vector<std::vector<int>> gridColors(rows, std::vector<int>(cols, 0));

    srand(static_cast<unsigned>(time(0)));

    // initializeGrid(gridColors, WALL_PROBABILITY);

    // Debugging: Print initial grid state
    // std::cout << "Initial Grid:\n";
    // for (auto row : gridColors) {
    //     for (auto cell : row) std::cout << (cell == 1 ? "#" : ".");
    //     std::cout << "\n";
    // }
    // std::cout << "\nApplying Cellular Automata...\n";

    // for (int i = 0; i < CA_STEPS; i++) {
    //     gridColors = applyCARules(gridColors);

    //     // Debugging: Print each step to check if grid changes
    //     std::cout << "Grid after step " << i + 1 << ":\n";
    //     for (auto row : gridColors) {
    //         for (auto cell : row) std::cout << (cell == 1 ? "#" : ".");
    //         std::cout << "\n";
    //     }
    //     std::cout << "\n";
    // }

    drunkWalk(gridColors, 150000);


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        sf::VertexArray grid(sf::Lines);


        // Draw grid lines
        for (int i = 0; i <= rows; ++i) {
            grid.append(sf::Vertex(sf::Vector2f(0, i * GRID_SPACING), sf::Color::Black));
            grid.append(sf::Vertex(sf::Vector2f(cols * GRID_SPACING, i * GRID_SPACING), sf::Color::Black));
        }
        for (int i = 0; i <= cols; ++i) {
            grid.append(sf::Vertex(sf::Vector2f(i * GRID_SPACING, 0), sf::Color::Black));
            grid.append(sf::Vertex(sf::Vector2f(i * GRID_SPACING, rows * GRID_SPACING), sf::Color::Black));
        }


        // Draw colored cells
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                sf::RectangleShape cell(sf::Vector2f(GRID_SPACING - 1, GRID_SPACING - 1));
                cell.setPosition(col * GRID_SPACING, row * GRID_SPACING);
                cell.setFillColor(gridColors[row][col] == 1 ? sf::Color::Black : sf::Color::White);
                window.draw(cell);
            }
        }

        window.draw(grid);
        window.display();
    }

    return 0;
}