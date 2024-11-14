#include <SFML/Graphics.hpp>
#include "mazegen.hpp"


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
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Procedural Maze Generation");

    int rows = window.getSize().y / GRID_SPACING;
    int cols = window.getSize().x / GRID_SPACING;

    std::vector<std::vector<int>> gridColors(rows, std::vector<int>(cols, 0));

    srand(static_cast<unsigned>(time(0)));

    // ------------------------------------ Cellular Automata Maze Generation ------------------------------------
    MazeGenerator* generator = new CellularAutomataGenerator(WALL_PROBABILITY, CA_STEPS);

    // ------------------------------------ Drunk Walk Maze Generation ------------------------------------
    // MazeGenerator* generator = new DrunkWalkGenerator(DRUNK_WALK_STEPS);

    // ------------------------------------ Prim's Maze Generation ------------------------------------
    // MazeGenerator* generator = new PrimGenerator();

    // ------------------------------------ L-System Maze Generation ------------------------------------
    // MazeGenerator* generator = new LSystemGenerator(L_SYSTEM_ITERATIONS, L_SYSTEM_STARTPOINTS);


    generator->generateMaze(gridColors, rows, cols);
    std::cout << "Maze generated!" << std::endl;

    // print the maze
    // for (int row = 0; row < rows; ++row) {
    //     for (int col = 0; col < cols; ++col) {
    //         std::cout << (gridColors[row][col] == WALL ? "#" : " ");
    //     }
    //     std::cout << std::endl;
    // }

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