#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "mazegen.hpp"


// Function to calculate the distance between two points
float calculateDistance(const sf::Vector2f& p1, const sf::Vector2f& p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

int main() {
    // Create a window with a specific size
    sf::RenderWindow window(sf::VideoMode(800, 800), "Simple SFML Window");

    // Get the desktop resolution and center the window
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    int centerX = (desktop.width - window.getSize().x) / 2;
    int centerY = (desktop.height - window.getSize().y) / 2;
    window.setPosition(sf::Vector2i(centerX, centerY));

    // Initialize the body part positions
    const float HIP_Y = 300.0f;
    const float KNEE_Y = HIP_Y + GRID_SPACING * 2;  // Knee is 2 grid spaces below the hip
    const float FEET_Y = KNEE_Y + GRID_SPACING * 2;  // Feet are 2 grid spaces below the knees

    sf::CircleShape hip(10);
    hip.setFillColor(sf::Color::Red);
    hip.setPosition(400 - hip.getRadius(), HIP_Y - hip.getRadius());  // Position hip

    sf::CircleShape leftKnee(8), rightKnee(8);
    leftKnee.setFillColor(sf::Color::Blue);
    rightKnee.setFillColor(sf::Color::Blue);
    leftKnee.setPosition(400 - GRID_SPACING - leftKnee.getRadius(), KNEE_Y - leftKnee.getRadius());  // Left knee
    rightKnee.setPosition(400 + GRID_SPACING - rightKnee.getRadius(), KNEE_Y - rightKnee.getRadius());  // Right knee

    sf::CircleShape leftFeet(6), rightFeet(6);
    leftFeet.setFillColor(sf::Color::Green);
    rightFeet.setFillColor(sf::Color::Green);
    leftFeet.setPosition(400 - GRID_SPACING - leftFeet.getRadius(), FEET_Y - leftFeet.getRadius());  // Left feet
    rightFeet.setPosition(400 + GRID_SPACING - rightFeet.getRadius(), FEET_Y - rightFeet.getRadius());  // Right feet

    // Define calf and thigh lengths
    float calfLengthLeft = calculateDistance(leftKnee.getPosition(), leftFeet.getPosition());
    float calfLengthRight = calculateDistance(rightKnee.getPosition(), rightFeet.getPosition());
    float thighLengthLeft = calculateDistance(hip.getPosition(), leftKnee.getPosition());
    float thighLengthRight = calculateDistance(hip.getPosition(), rightKnee.getPosition());

    bool moveLeftFoot = true;  // Toggle between moving left and right foot

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Key press handling for foot movement (Right movement with D, Left movement with A)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (moveLeftFoot) {
                // Move left foot first (right movement)
                sf::Vector2f leftFeetPos = leftFeet.getPosition();
                leftFeetPos.x += GRID_SPACING;  // Move left foot right
                leftFeet.setPosition(leftFeetPos.x, leftFeetPos.y);

                // Update left knee to maintain calf length
                sf::Vector2f leftKneePos = leftKnee.getPosition();
                float calfDistance = calculateDistance(leftKneePos, leftFeet.getPosition());
                float calfRatio = calfLengthLeft / calfDistance;
                leftKneePos.x = leftFeet.getPosition().x - (leftFeet.getPosition().x - leftKneePos.x) * calfRatio;
                leftKneePos.y = leftFeet.getPosition().y - (leftFeet.getPosition().y - leftKneePos.y) * calfRatio;
                leftKnee.setPosition(leftKneePos.x - leftKnee.getRadius(), leftKneePos.y - leftKnee.getRadius());

                // Update hip to maintain thigh length
                sf::Vector2f hipPos = hip.getPosition();
                float thighDistance = calculateDistance(hipPos, leftKnee.getPosition());
                float thighRatio = thighLengthLeft / thighDistance;
                hipPos.x = leftKnee.getPosition().x - (leftKnee.getPosition().x - hipPos.x) * thighRatio;
                hipPos.y = leftKnee.getPosition().y - (leftKnee.getPosition().y - hipPos.y) * thighRatio;
                hip.setPosition(hipPos.x - hip.getRadius(), hipPos.y - hip.getRadius());

                moveLeftFoot = false;  // Next move will be for the right foot
            } else {
                // Move right foot (right movement)
                sf::Vector2f rightFeetPos = rightFeet.getPosition();
                rightFeetPos.x += GRID_SPACING;  // Move right foot right
                rightFeet.setPosition(rightFeetPos.x, rightFeetPos.y);

                // Update right knee to maintain calf length
                sf::Vector2f rightKneePos = rightKnee.getPosition();
                float calfDistance = calculateDistance(rightKneePos, rightFeet.getPosition());
                float calfRatio = calfLengthRight / calfDistance;
                rightKneePos.x = rightFeet.getPosition().x - (rightFeet.getPosition().x - rightKneePos.x) * calfRatio;
                rightKneePos.y = rightFeet.getPosition().y - (rightFeet.getPosition().y - rightKneePos.y) * calfRatio;
                rightKnee.setPosition(rightKneePos.x - rightKnee.getRadius(), rightKneePos.y - rightKnee.getRadius());

                // Update hip to maintain thigh length
                sf::Vector2f hipPos = hip.getPosition();
                float thighDistance = calculateDistance(hipPos, rightKnee.getPosition());
                float thighRatio = thighLengthRight / thighDistance;
                hipPos.x = rightKnee.getPosition().x - (rightKnee.getPosition().x - hipPos.x) * thighRatio;
                hipPos.y = rightKnee.getPosition().y - (rightKnee.getPosition().y - hipPos.y) * thighRatio;
                hip.setPosition(hipPos.x - hip.getRadius(), hipPos.y - hip.getRadius());

                moveLeftFoot = true;  // Next move will be for the left foot
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (!moveLeftFoot) {
                // Move left foot first (left movement)
                sf::Vector2f leftFeetPos = leftFeet.getPosition();
                leftFeetPos.x -= GRID_SPACING;  // Move left foot left
                leftFeet.setPosition(leftFeetPos.x, leftFeetPos.y);

                // Update left knee to maintain calf length
                sf::Vector2f leftKneePos = leftKnee.getPosition();
                float calfDistance = calculateDistance(leftKneePos, leftFeet.getPosition());
                float calfRatio = calfLengthLeft / calfDistance;
                leftKneePos.x = leftFeet.getPosition().x - (leftFeet.getPosition().x - leftKneePos.x) * calfRatio;
                leftKneePos.y = leftFeet.getPosition().y - (leftFeet.getPosition().y - leftKneePos.y) * calfRatio;
                leftKnee.setPosition(leftKneePos.x - leftKnee.getRadius(), leftKneePos.y - leftKnee.getRadius());

                // Update hip to maintain thigh length
                sf::Vector2f hipPos = hip.getPosition();
                float thighDistance = calculateDistance(hipPos, leftKnee.getPosition());
                float thighRatio = thighLengthLeft / thighDistance;
                hipPos.x = leftKnee.getPosition().x - (leftKnee.getPosition().x - hipPos.x) * thighRatio;
                hipPos.y = leftKnee.getPosition().y - (leftKnee.getPosition().y - hipPos.y) * thighRatio;
                hip.setPosition(hipPos.x - hip.getRadius(), hipPos.y - hip.getRadius());

                moveLeftFoot = true;  // Next move will be for the right foot
            } else {
                // Move right foot (left movement)
                sf::Vector2f rightFeetPos = rightFeet.getPosition();
                rightFeetPos.x -= GRID_SPACING;  // Move right foot left
                rightFeet.setPosition(rightFeetPos.x, rightFeetPos.y);

                // Update right knee to maintain calf length
                sf::Vector2f rightKneePos = rightKnee.getPosition();
                float calfDistance = calculateDistance(rightKneePos, rightFeet.getPosition());
                float calfRatio = calfLengthRight / calfDistance;
                rightKneePos.x = rightFeet.getPosition().x - (rightFeet.getPosition().x - rightKneePos.x) * calfRatio;
                rightKneePos.y = rightFeet.getPosition().y - (rightFeet.getPosition().y - rightKneePos.y) * calfRatio;
                rightKnee.setPosition(rightKneePos.x - rightKnee.getRadius(), rightKneePos.y - rightKnee.getRadius());

                // Update hip to maintain thigh length
                sf::Vector2f hipPos = hip.getPosition();
                float thighDistance = calculateDistance(hipPos, rightKnee.getPosition());
                float thighRatio = thighLengthRight / thighDistance;
                hipPos.x = rightKnee.getPosition().x - (rightKnee.getPosition().x - hipPos.x) * thighRatio;
                hipPos.y = rightKnee.getPosition().y - (rightKnee.getPosition().y - hipPos.y) * thighRatio;
                hip.setPosition(hipPos.x - hip.getRadius(), hipPos.y - hip.getRadius());

                moveLeftFoot = false;  // Next move will be for the left foot
            }
        }

        // Clear the window
        window.clear(sf::Color::White);

        // Draw grid lines
        sf::VertexArray grid(sf::Lines);
        int rows = window.getSize().y / GRID_SPACING;
        int cols = window.getSize().x / GRID_SPACING;
        for (int i = 0; i <= rows; ++i) {
            grid.append(sf::Vertex(sf::Vector2f(0, i * GRID_SPACING), sf::Color::Black));
            grid.append(sf::Vertex(sf::Vector2f(cols * GRID_SPACING, i * GRID_SPACING), sf::Color::Black));
        }
        for (int i = 0; i <= cols; ++i) {
            grid.append(sf::Vertex(sf::Vector2f(i * GRID_SPACING, 0), sf::Color::Black));
            grid.append(sf::Vertex(sf::Vector2f(i * GRID_SPACING, rows * GRID_SPACING), sf::Color::Black));
        }
        window.draw(grid);

        // Draw the body parts
        window.draw(hip);
        window.draw(leftKnee);
        window.draw(rightKnee);
        window.draw(leftFeet);
        window.draw(rightFeet);

        // Display everything
        window.display();
    }

    return 0;
}
