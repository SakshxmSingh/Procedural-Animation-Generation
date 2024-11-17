#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

// Distance b/w 2 points
float calculateDistance(const sf::Vector2f& p1, const sf::Vector2f& p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

// finding possible knee positions
sf::Vector2f findKneePosition(const sf::Vector2f& hipCenter, const sf::Vector2f& feetCenter,
                               float thighLength, float calfLength, const sf::Vector2f& currentKnee) {
    // Vector from hip to feet
    sf::Vector2f hfVec = feetCenter - hipCenter;
    float hfDistance = calculateDistance(hipCenter, feetCenter);

    // Check if the position is valid
    if (hfDistance > (thighLength + calfLength) || hfDistance < std::abs(thighLength - calfLength)) {
        throw std::invalid_argument("Invalid feet position: Constraints cannot be satisfied.");
    }

    // Use the circle intersection formula to calculate possible knee positions
    float d = hfDistance;
    float a = (thighLength * thighLength - calfLength * calfLength + d * d) / (2 * d);
    float h = std::sqrt(thighLength * thighLength - a * a);

    // Midpoint on the line from hip to feet
    sf::Vector2f midpoint = hipCenter + a * (hfVec / d);

    // Perpendicular vector for the knee
    sf::Vector2f perpendicularVec(-hfVec.y / d, hfVec.x / d);

    // Two possible knee positions
    sf::Vector2f knee1 = midpoint + h * perpendicularVec;
    sf::Vector2f knee2 = midpoint - h * perpendicularVec;

    // Choose the knee position closest to the current knee position
    float dist1 = calculateDistance(knee1, currentKnee);
    float dist2 = calculateDistance(knee2, currentKnee);

    return (dist1 < dist2) ? knee1 : knee2;
}

// Function to create a circle with given parameters
sf::CircleShape createCircle(float radius, const sf::Color& color, const sf::Vector2f& position) {
    sf::CircleShape circle(radius);
    circle.setFillColor(color);
    circle.setPosition(position - sf::Vector2f(radius, radius));
    return circle;
}

// Function to draw the connecting lines
void drawLines(sf::RenderWindow& window, const sf::CircleShape& hip, const sf::CircleShape& leftKnee, const sf::CircleShape& rightKnee,
               const sf::CircleShape& leftFeet, const sf::CircleShape& rightFeet) {
    sf::VertexArray lines(sf::Lines, 8);

    // Define the lines connecting body parts
    sf::Vector2f hipCenter = hip.getPosition() + sf::Vector2f(hip.getRadius(), hip.getRadius());
    sf::Vector2f leftKneeCenter = leftKnee.getPosition() + sf::Vector2f(leftKnee.getRadius(), leftKnee.getRadius());
    sf::Vector2f rightKneeCenter = rightKnee.getPosition() + sf::Vector2f(rightKnee.getRadius(), rightKnee.getRadius());
    sf::Vector2f leftFeetCenter = leftFeet.getPosition() + sf::Vector2f(leftFeet.getRadius(), leftFeet.getRadius());
    sf::Vector2f rightFeetCenter = rightFeet.getPosition() + sf::Vector2f(rightFeet.getRadius(), rightFeet.getRadius());

    // Connect hip to knees
    lines[0].position = hipCenter; lines[0].color = sf::Color::Red;
    lines[1].position = leftKneeCenter; lines[1].color = sf::Color::Red;

    lines[2].position = hipCenter; lines[2].color = sf::Color::Red;
    lines[3].position = rightKneeCenter; lines[3].color = sf::Color::Red;

    // Connect knees to feet
    lines[4].position = leftKneeCenter; lines[4].color = sf::Color::Red;
    lines[5].position = leftFeetCenter; lines[5].color = sf::Color::Red;

    lines[6].position = rightKneeCenter; lines[6].color = sf::Color::Red;
    lines[7].position = rightFeetCenter; lines[7].color = sf::Color::Red;

    window.draw(lines);
}

// Handle dragging logic
void handleDragging(sf::RenderWindow& window, sf::CircleShape* draggedObject, sf::Vector2f& offset,
                    const sf::CircleShape& hip, sf::CircleShape& leftKnee, sf::CircleShape& rightKnee,
                    sf::CircleShape& leftFeet, sf::CircleShape& rightFeet,
                    float thighLength, float calfLength) {
    if (draggedObject != nullptr) {
        // Update the dragged object position based on mouse
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        draggedObject->setPosition(mousePos - offset);

        // Get positions of other key points
        sf::Vector2f hipCenter = hip.getPosition() + sf::Vector2f(hip.getRadius(), hip.getRadius());

        if (draggedObject == &leftFeet) {
            sf::Vector2f feetCenter = leftFeet.getPosition() + sf::Vector2f(leftFeet.getRadius(), leftFeet.getRadius());
            sf::Vector2f currentKnee = leftKnee.getPosition() + sf::Vector2f(leftKnee.getRadius(), leftKnee.getRadius());

            // Find the new knee position
            sf::Vector2f newKnee = findKneePosition(hipCenter, feetCenter, thighLength, calfLength, currentKnee);

            // Update the knee's position
            leftKnee.setPosition(newKnee - sf::Vector2f(leftKnee.getRadius(), leftKnee.getRadius()));
        } else if (draggedObject == &rightFeet) {
            sf::Vector2f feetCenter = rightFeet.getPosition() + sf::Vector2f(rightFeet.getRadius(), rightFeet.getRadius());
            sf::Vector2f currentKnee = rightKnee.getPosition() + sf::Vector2f(rightKnee.getRadius(), rightKnee.getRadius());

            // Find the new knee position
            sf::Vector2f newKnee = findKneePosition(hipCenter, feetCenter, thighLength, calfLength, currentKnee);

            // Update the knee's position
            rightKnee.setPosition(newKnee - sf::Vector2f(rightKnee.getRadius(), rightKnee.getRadius()));
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Simple SFML Window");
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.setPosition(sf::Vector2i((desktop.width - window.getSize().x) / 2, (desktop.height - window.getSize().y) / 2));

    // body parts
    sf::CircleShape hip = createCircle(20, sf::Color::Red, {400, 300});
    sf::CircleShape leftKnee = createCircle(8, sf::Color::Blue, {360, 340});
    sf::CircleShape rightKnee = createCircle(8, sf::Color::Blue, {440, 340});
    sf::CircleShape leftFeet = createCircle(6, sf::Color::Green, {360, 380});
    sf::CircleShape rightFeet = createCircle(6, sf::Color::Green, {440, 380});

    // Calculate lengths
    float thighLength = calculateDistance(hip.getPosition() + sf::Vector2f(hip.getRadius(), hip.getRadius()),
                                          leftKnee.getPosition() + sf::Vector2f(leftKnee.getRadius(), leftKnee.getRadius()));
    float calfLength = calculateDistance(leftKnee.getPosition() + sf::Vector2f(leftKnee.getRadius(), leftKnee.getRadius()),
                                          leftFeet.getPosition() + sf::Vector2f(leftFeet.getRadius(), leftFeet.getRadius()));

    std::cout << "Thigh Length: " << thighLength << "\n";
    std::cout << "Calf Length: " << calfLength << "\n";

    // Dragging state variables
    sf::CircleShape* draggedObject = nullptr;
    sf::Vector2f offset;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (hip.getGlobalBounds().contains(mousePos)) draggedObject = &hip, offset = mousePos - hip.getPosition();
                else if (leftKnee.getGlobalBounds().contains(mousePos)) draggedObject = &leftKnee, offset = mousePos - leftKnee.getPosition();
                else if (rightKnee.getGlobalBounds().contains(mousePos)) draggedObject = &rightKnee, offset = mousePos - rightKnee.getPosition();
                else if (leftFeet.getGlobalBounds().contains(mousePos)) draggedObject = &leftFeet, offset = mousePos - leftFeet.getPosition();
                else if (rightFeet.getGlobalBounds().contains(mousePos)) draggedObject = &rightFeet, offset = mousePos - rightFeet.getPosition();
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                draggedObject = nullptr;
            }
        }

        handleDragging(window, draggedObject, offset, hip, leftKnee, rightKnee, leftFeet, rightFeet, thighLength, calfLength);

        window.clear(sf::Color::White);
        drawLines(window, hip, leftKnee, rightKnee, leftFeet, rightFeet);
        window.draw(hip);
        window.draw(leftKnee);
        window.draw(rightKnee);
        window.draw(leftFeet);
        window.draw(rightFeet);
        window.display();
    }

    return 0;
}
