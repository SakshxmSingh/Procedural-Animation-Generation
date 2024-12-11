#include "mazegen.hpp"
#include "spider.hpp"

#define MOVE_DURATION 60000 // Duration of player movement in microseconds


// Function to find the first open cell (PATH) from the bottom-left of the grid
sf::Vector2f findStartingPosition(const std::vector<std::vector<int>>& gridColors, int rows, int cols) {  //BFS
    std::queue<Cell> q;
    q.push(Cell(rows - 1, 0)); // Start from the bottom-left corner

    while (!q.empty()) {
        Cell current = q.front();
        q.pop();

        if (gridColors[current.row][current.col] == PATH) {
            return sf::Vector2f(current.col * GRID_SPACING, current.row * GRID_SPACING);
        }

        std::vector<Cell> neighbors = {
            {current.row - 1, current.col}, 
            {current.row + 1, current.col}, 
            {current.row, current.col - 1}, 
            {current.row, current.col + 1}  
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
    sf::RenderWindow window(sf::VideoMode(800, 800), "Maze spider");

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

    // with 10% chance, make a wall a light
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (gridColors[row][col] == WALL && rand() % 60 == 0) {
                gridColors[row][col] = LIGHT;
            }
        }
    }

    sf::Texture wallTexture;
    if(!wallTexture.loadFromFile("assets/grey-wall.png")) {
        std::cerr << "Failed to load wall texture!" << std::endl;
        return 1;
    }

    sf::Texture surfaceTexture;
    if(!surfaceTexture.loadFromFile("assets/grey-surface.png")) {
        std::cerr << "Failed to load surface texture!" << std::endl;
        return 1;
    }

    sf::Texture backgroundTexture;
    if(!backgroundTexture.loadFromFile("assets/background.png")) {
        std::cerr << "Failed to load background texture!" << std::endl;
        return 1;
    }

    sf::Texture lightTexture;
    if(!lightTexture.loadFromFile("assets/light.png")) {
        std::cerr << "Failed to load light texture!" << std::endl;
        return 1;
    }

    // make the whole maze a path
    // for (int row = 1; row < rows - 1; ++row) {
    //     for (int col = 1; col < cols - 1; ++col) {
    //         gridColors[row][col] = PATH;
    //     }
    // }

    // print the maze
    // for (int row = 0; row < rows; ++row) {
    //     for (int col = 0; col < cols; ++col) {
    //         std::cout << (gridColors[row][col] == WALL ? "#" : " ");
    //     }
    //     std::cout << std::endl;
    // }

    // ------------------------------------ Player Movement ------------------------------------
    sf::RectangleShape player(sf::Vector2f(GRID_SPACING, GRID_SPACING));
    player.setFillColor(sf::Color::Red);

    sf::Vector2f playerPos = findStartingPosition(gridColors, rows, cols);
    player.setPosition(playerPos);

    std::cout << "Player starting position: " << playerPos.x << ", " << playerPos.y << std::endl;

    bool isFalling = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // Linearly interpolate the position between old and new, such that speed is equal to 10000us
        static sf::Vector2f startPos;
        static sf::Vector2f endPos;
        static bool isMoving = false;
        static sf::Clock moveClock;
        const sf::Time moveDuration = sf::microseconds(MOVE_DURATION);

        static float fallingSpeed = 0.0f; // Initialize falling speed
        const float GRAVITY = 0.01f; // Gravity acceleration per frame
        const float TERM_VELO = 5.0f; // Maximum falling speed
        

        // ---------------------------------------- Player Movement ----------------------------------------
        if (!isMoving) {
            sf::Vector2f playerNewPos = player.getPosition();

            bool keyPressed = false;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !isFalling) {
                playerNewPos.y -= GRID_SPACING;
                playerNewPos.x -= GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !isFalling) {
                playerNewPos.y -= GRID_SPACING;
                playerNewPos.x += GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                playerNewPos.y += GRID_SPACING;
                playerNewPos.x -= GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                playerNewPos.y += GRID_SPACING;
                playerNewPos.x += GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !isFalling) {
                playerNewPos.y -= GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                playerNewPos.y += GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                playerNewPos.x -= GRID_SPACING;
                keyPressed = true;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                playerNewPos.x += GRID_SPACING;
                keyPressed = true;
            }

            int newRow = static_cast<int>(playerNewPos.y / GRID_SPACING);
            int newCol = static_cast<int>(playerNewPos.x / GRID_SPACING);

            if (keyPressed && isInBounds(newRow, newCol, rows, cols) && gridColors[newRow][newCol] != WALL && gridColors[newRow][newCol] != LIGHT) {
                startPos = player.getPosition();
                endPos = sf::Vector2f(newCol * GRID_SPACING, newRow * GRID_SPACING);
                moveClock.restart();
                isMoving = true;
            }
        }

        if (isMoving) {
            float t = moveClock.getElapsedTime().asMicroseconds() / static_cast<float>(moveDuration.asMicroseconds());
            if (t >= 1.f) {
                t = 1.f;
                isMoving = false;
            }
            sf::Vector2f currentPos = startPos + t * (endPos - startPos);
            player.setPosition(currentPos);
        }

        std::vector<Limb> limbs;
        std::vector<sf::Vector2f> hexagonPoints = getHexagonalPoints(player.getPosition());

        // ---------------------------------- Limb and Limb Guidelines Animation ----------------------------------
        for (const auto& point : hexagonPoints) {
            sf::Vector2f direction = point - player.getPosition() + sf::Vector2f(GRID_SPACING / 2, GRID_SPACING / 2);
            sf::Vector2f wallPos = findClosestWall(player.getPosition() + sf::Vector2f(GRID_SPACING / 2, GRID_SPACING / 2), direction, gridColors, rows, cols);
            limbs.emplace_back(player.getPosition() + sf::Vector2f(GRID_SPACING / 2, GRID_SPACING / 2), wallPos);
        }

        sf::VertexArray guideLines(sf::Lines);
        for (const auto& point : hexagonPoints) {
            guideLines.append(sf::Vertex(player.getPosition() + sf::Vector2f(player.getSize().x / 2, player.getSize().y / 2), sf::Color(225,135, 0)));
            guideLines.append(sf::Vertex(point, sf::Color(225, 135, 0))); // Orange color
        }

        for (Limb& limb : limbs) {
            limb.animate(1.f); // Animate the limb
        }

        // ---------------------------------------- Falling ----------------------------------------
        // Count active limbs
        int activeLimbs = 0;
        for (const auto& limb : limbs) {
            if (limb.active) {
                activeLimbs++;
            }
        }
        if (activeLimbs == 0) {
            // No limbs connected: full GRAVITY
            fallingSpeed += GRAVITY;
            isFalling = true;
        } else if (activeLimbs < 3) {
            // Less than 3 limbs: scaled GRAVITY
            fallingSpeed += GRAVITY / activeLimbs;
            isFalling = true;
        } else {
            fallingSpeed = 0.0f;
            isFalling = false;
        }
        if (fallingSpeed > TERM_VELO) {
            fallingSpeed = TERM_VELO;
        }
        player.move(0, fallingSpeed);

        // Prevent the player from falling below the grid
        if (player.getPosition().y > rows * GRID_SPACING) {
            player.setPosition(player.getPosition().x, rows * GRID_SPACING - GRID_SPACING);
            fallingSpeed = 0.0f; // Reset falling speed
            isFalling = false;
        }



        // ---------------------------------------- Drawing ----------------------------------------
        window.clear(sf::Color::White);
        sf::VertexArray grid(sf::Lines);

        // Draw gridlines
        for (int i = 0; i <= rows; ++i) {
            grid.append(sf::Vertex(sf::Vector2f(0, i * GRID_SPACING), sf::Color::Black));
            grid.append(sf::Vertex(sf::Vector2f(cols * GRID_SPACING, i * GRID_SPACING), sf::Color::Black));
        }
        for (int i = 0; i <= cols; ++i) {
            grid.append(sf::Vertex(sf::Vector2f(i * GRID_SPACING, 0), sf::Color::Black));
            grid.append(sf::Vertex(sf::Vector2f(i * GRID_SPACING, rows * GRID_SPACING), sf::Color::Black));
        }


        // Draw colored cells
        // for (int row = 0; row < rows; ++row) {
        //     for (int col = 0; col < cols; ++col) {
        //         sf::RectangleShape cell(sf::Vector2f(GRID_SPACING, GRID_SPACING));
        //         cell.setPosition(col * GRID_SPACING, row * GRID_SPACING);
        //         cell.setFillColor(gridColors[row][col] == PATH ? sf::Color::Black : sf::Color::White);
        //         window.draw(cell);
        //     }
        // }

        // window.draw(grid);

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (gridColors[row][col] == WALL) {
                    sf::Sprite cellSprite;
                    if (row > 0 && (gridColors[row - 1][col] == WALL || gridColors[row - 1][col] == LIGHT)) {
                        cellSprite.setTexture(wallTexture);
                    } else {
                        cellSprite.setTexture(surfaceTexture);
                    }
                    cellSprite.setPosition(col * GRID_SPACING, row * GRID_SPACING);
                    cellSprite.setScale(
                        GRID_SPACING / static_cast<float>(cellSprite.getTexture()->getSize().x),
                        GRID_SPACING / static_cast<float>(cellSprite.getTexture()->getSize().y)
                    );
                    window.draw(cellSprite);
                } else if (gridColors[row][col] == PATH) {
                    // sf::RectangleShape cell(sf::Vector2f(GRID_SPACING, GRID_SPACING));
                    // cell.setPosition(col * GRID_SPACING, row * GRID_SPACING);
                    // cell.setFillColor(sf::Color::Black);
                    // window.draw(cell);
                    sf::Sprite cellSprite;
                    cellSprite.setTexture(backgroundTexture);
                    cellSprite.setPosition(col * GRID_SPACING, row * GRID_SPACING);
                    cellSprite.setScale(
                        GRID_SPACING / static_cast<float>(cellSprite.getTexture()->getSize().x),
                        GRID_SPACING / static_cast<float>(cellSprite.getTexture()->getSize().y)
                    );
                    window.draw(cellSprite);
                } else if (gridColors[row][col] == LIGHT) {
                    sf::Sprite cellSprite;
                    cellSprite.setTexture(lightTexture);
                    cellSprite.setPosition(col * GRID_SPACING, row * GRID_SPACING);
                    cellSprite.setScale(
                        GRID_SPACING / static_cast<float>(cellSprite.getTexture()->getSize().x),
                        GRID_SPACING / static_cast<float>(cellSprite.getTexture()->getSize().y)
                    );
                    window.draw(cellSprite);
                }
            }
        }


        // window.draw(guideLines);
        window.draw(player);

        for (const auto& point : hexagonPoints) {
            sf::CircleShape circle(CIRCLE_RADIUS);
            circle.setFillColor(sf::Color::Blue);
            circle.setPosition(point.x - CIRCLE_RADIUS, point.y - CIRCLE_RADIUS);
            // window.draw(circle);
        }

        sf::VertexArray limbLines(sf::Lines);
        for (const auto& limb : limbs) {
            if(!limb.active) continue;
            limbLines.append(sf::Vertex(limb.start, sf::Color::Red));
            limbLines.append(sf::Vertex(limb.end, sf::Color::Red));
        }
        window.draw(limbLines);

        window.display();
    }

    return 0;
}