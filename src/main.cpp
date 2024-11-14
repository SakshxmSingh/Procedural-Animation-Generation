#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For seeding rand()
#include <iostream>
#include <stack>
#include <algorithm> 
#include <unordered_map>
#include <queue>
#include <unistd.h>


#define GRID_SPACING 10 // Size of each cell (40x40 pixels)
#define WALL_PROBABILITY 0.64 // Probability of a cell being a wall
#define CA_STEPS 5 // Number of Cellular Automata steps
#define WALL 0
#define PATH 1

struct Cell {
    int row, col;
    Cell(int row, int col) : row(row), col(col) {}
    bool isInsideGrid(int rows, int cols) const {
        return row >= 0 && row < rows && col >= 0 && col < cols;
    }
};

bool isInBounds(int row, int col, int rows, int cols) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

void addFrontier(const Cell& cell, const std::vector<std::vector<int>>& grid, std::vector<Cell>& frontier) {
    int rows = grid.size();
    int cols = grid[0].size();
    std::vector<Cell> neighbors = {
        {cell.row - 2, cell.col},
        {cell.row + 2, cell.col},
        {cell.row, cell.col - 2},
        {cell.row, cell.col + 2}
    };

    for (const Cell& neighbor : neighbors) {
        if (neighbor.isInsideGrid(rows, cols) && grid[neighbor.row][neighbor.col] == WALL) {
            frontier.push_back(neighbor);
        }
    }
}

void PrimMaze(std::vector<std::vector<int>>& grid) {
    int rows = grid.size();
    int cols = grid[0].size();
    std::vector<Cell> frontier;

    // Start at a random cell and mark it as a path
    Cell start(1, 1);
    grid[start.row][start.col] = PATH;
    addFrontier(start, grid, frontier);

    // Randomly process frontier cells
    while (!frontier.empty()) {
        int idx = rand() % frontier.size();
        Cell cell = frontier[idx];
        frontier.erase(frontier.begin() + idx);

        // Get neighboring path cells
        std::vector<Cell> neighbors;
        if (isInBounds(cell.row - 2, cell.col, rows, cols) && grid[cell.row - 2][cell.col] == PATH)
            neighbors.push_back({cell.row - 2, cell.col});
        if (isInBounds(cell.row + 2, cell.col, rows, cols) && grid[cell.row + 2][cell.col] == PATH)
            neighbors.push_back({cell.row + 2, cell.col});
        if (isInBounds(cell.row, cell.col - 2, rows, cols) && grid[cell.row][cell.col - 2] == PATH)
            neighbors.push_back({cell.row, cell.col - 2});
        if (isInBounds(cell.row, cell.col + 2, rows, cols) && grid[cell.row][cell.col + 2] == PATH)
            neighbors.push_back({cell.row, cell.col + 2});

        // If there is a neighboring path, carve a passage
        if (!neighbors.empty()) {
            Cell neighbor = neighbors[rand() % neighbors.size()];
            grid[cell.row][cell.col] = PATH;
            grid[(cell.row + neighbor.row) / 2][(cell.col + neighbor.col) / 2] = PATH;
            addFrontier(cell, grid, frontier);
        }
    }
}

void drunkWalk(std::vector<std::vector<int>>& gridColors, int steps) {
    int rows = gridColors.size();
    int cols = gridColors[0].size();
    
    // Start position (randomly chosen)
    // int row = rand() % rows;
    // int col = rand() % cols;

    // // start at the bottom left corner
    int row = rows - 1;
    int col = 0;

    // randomly decide the corner to start at
    // int corner = rand() % 4;
    // int row, col;
    // if (corner == 0) {
    //     row = rows - 1;
    //     col = 0;
    // } else if (corner == 1) {
    //     row = 0;
    //     col = 0;
    // } else if (corner == 2) {
    //     row = 0;
    //     col = cols - 1;
    // } else {
    //     row = rows - 1;
    //     col = cols - 1;
    // }
    
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

//     // Set the outer border to be walls
//     for (int i = 0; i < grid.size(); ++i) {
//         grid[i][0] = 1;
//         grid[i][grid[0].size() - 1] = 1;
//     }
//     for (int i = 0; i < grid[0].size(); ++i) {
//         grid[0][i] = 1;
//         grid[grid.size() - 1][i] = 1;
//     }

//     // Set the start and end points
//     grid[grid.size() - 1][0] = 0;
//     grid[0][grid[0].size() - 1] = 0;
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


std::string evolveLSystem(const std::string& axiom, const std::unordered_map<char, std::vector<std::string>>& rules, int iterations) {
    std::string result = axiom;
    for (int i = 0; i < iterations; ++i) {
        std::string next;
        for (char c : result) {
            if (rules.find(c) != rules.end()) {
                const std::vector<std::string>& possibleRules = rules.at(c);
                next += possibleRules[rand() % possibleRules.size()]; // Randomly select a rule
            } else {
                next += c;
            }
        }
        result = next;
    }
    return result;
}

void interpretLSystem(const std::string& instructions, std::vector<std::vector<int>>& grid, int startRow, int startCol) {
    int rows = grid.size();
    int cols = grid[0].size();
    int direction = 0; // 0=up, 1=right, 2=down, 3=left
    int row = startRow;
    int col = startCol;

    grid[row][col] = PATH;

    for (char command : instructions) {
        if (command == 'F') {
            // Move in the current direction
            if (direction == 0 && row > 0) row--;       // Move up
            else if (direction == 1 && col < cols - 1) col++; // Move right
            else if (direction == 2 && row < rows - 1) row++; // Move down
            else if (direction == 3 && col > 0) col--;       // Move left
            grid[row][col] = PATH;
        } else if (command == '+') {
            direction = (direction + 1) % 4; // Turn right
        } else if (command == '-') {
            direction = (direction + 3) % 4; // Turn left
        }
    }
}

// Function to find the first open cell (PATH) from the bottom-left of the grid
sf::Vector2f findStartingPosition(const std::vector<std::vector<int>>& gridColors, int rows, int cols) {
    // Start from the bottom row and move upwards in the leftmost column (column 0)
    std::queue<Cell> q;
    q.push(Cell(rows - 1, 0)); // Start from the bottom-left corner

    while (!q.empty()) {
        Cell current = q.front();
        q.pop();

        if (gridColors[current.row][current.col] == PATH) {
            return sf::Vector2f(current.col * GRID_SPACING, current.row * GRID_SPACING);
        }

        // Explore neighbors in BFS fashion
        std::vector<Cell> neighbors = {
            {current.row - 1, current.col}, // Up
            {current.row + 1, current.col}, // Down
            {current.row, current.col - 1}, // Left
            {current.row, current.col + 1}  // Right
        };

        for (const Cell& neighbor : neighbors) {
            if (neighbor.isInsideGrid(rows, cols)) {
                q.push(neighbor);
            }
        }
    }

    // Default to top-left if no open cell is found
    return sf::Vector2f(0, 0);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Cellular Automata Maze");

    int rows = window.getSize().y / GRID_SPACING;
    int cols = window.getSize().x / GRID_SPACING;

    std::vector<std::vector<int>> gridColors(rows, std::vector<int>(cols, 0));

    srand(static_cast<unsigned>(time(0)));

    // ------------------------------------ Cellular Automata Maze Generation ------------------------------------
    initializeGrid(gridColors, WALL_PROBABILITY);

    for (int i = 0; i < CA_STEPS; i++) {
        gridColors = applyCARules(gridColors);
    }

    // ------------------------------------ Drunk Walk Maze Generation ------------------------------------
    // drunkWalk(gridColors, 30000);

    // ------------------------------------ Prim's Maze Generation ------------------------------------
    // PrimMaze(gridColors);

    // ------------------------------------ L-System Maze Generation ------------------------------------
    // std::unordered_map<char, std::vector<std::string>> rules;
    // rules['F'] = {"F+F-F-F+F", "F-F+F+F-F", "F-F-F+F+F"}; // Multiple rules for randomness
    // rules['+'] = {"+", "-"}; // Turn right or left
    // rules['-'] = {"-", "+"}; // Turn left or right

    // // Generate the L-system string
    // std::string axiom = "F";
    // int iterations = 4; // Adjust based on the complexity you want

    // for (int i = 0; i < 5 ; i++) {
    //     std::string instructions = evolveLSystem(axiom, rules, iterations);
    //     if(i == 0){
    //         interpretLSystem(instructions, gridColors, rows - 1, 0);
    //     }
    //     else{
    //         // cjose start randomly
    //         int row = rand() % rows;
    //         int col = rand() % cols;
    //         interpretLSystem(instructions, gridColors, row, col);
    //     }
        
    // }


    std::cout << "Maze generated!" << std::endl;
    // ------------------------------------ Player Movement ------------------------------------
    sf::RectangleShape player(sf::Vector2f(GRID_SPACING - 2, GRID_SPACING - 2));
    player.setFillColor(sf::Color::Red);

    sf::Vector2f playerPos = findStartingPosition(gridColors, rows, cols);
    player.setPosition(playerPos);

    std::cout << "Player starting position: " << playerPos.x << ", " << playerPos.y << std::endl;

    float playerSpeed = GRID_SPACING / 10.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Player movement with snapping to grid
        sf::Vector2f playerNewPos = player.getPosition();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) // Move up
            playerNewPos.y -= GRID_SPACING;
            // add sleep call in order to slow down the player movement for 200ms
            usleep(10000); // Sleep for 20 milliseconds
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) // Move down
            playerNewPos.y += GRID_SPACING;
            usleep(10000); // Sleep for 20 milliseconds
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) // Move left
            playerNewPos.x -= GRID_SPACING;
            usleep(10000); // Sleep for 20 milliseconds
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) // Move right
            playerNewPos.x += GRID_SPACING;
            usleep(10000); // Sleep for 20 milliseconds

        // Convert player position to grid coordinates for collision checking
        int newRow = static_cast<int>(playerNewPos.y / GRID_SPACING);
        int newCol = static_cast<int>(playerNewPos.x / GRID_SPACING);

        // Ensure new position is within grid and check for wall collision
        if (isInBounds(newRow, newCol, rows, cols) && gridColors[newRow][newCol] != WALL) {
            player.setPosition(newCol * GRID_SPACING, newRow * GRID_SPACING); // Snap to grid position if no collision
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
                cell.setFillColor(gridColors[row][col] == PATH ? sf::Color::Black : sf::Color::White);
                window.draw(cell);
            }
        }

        window.draw(grid);
        window.draw(player);
        window.display();
    }

    return 0;
}