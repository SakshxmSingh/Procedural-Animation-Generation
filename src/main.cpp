#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <unistd.h>
#include <cstdlib> // For rand()
#include <ctime>   // For seeding rand()

// Global body parts
sf::CircleShape hip;
sf::CircleShape leftKnee;
sf::CircleShape rightKnee;
sf::CircleShape leftFeet;
sf::CircleShape rightFeet;

// upper body
sf::CircleShape chest;

int whichFeet = 0; // 0 for left and 1 for right
std::string controlForm = "Key"; // "Mouse" or "Key"
int loopCounter = 0;
int onesteponefoot = 25;

// Distance b/w 2 points
float calculateDistance(const sf::Vector2f& p1, const sf::Vector2f& p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

// Function to compute a point using polar coordinates
sf::Vector2f calculatePoint(const sf::Vector2f& origin, float distance, float angle) {
    return sf::Vector2f(
        origin.x + distance * std::cos(angle),
        origin.y + distance * std::sin(angle)
    );
}

sf::CircleShape createCircle(float radius, const sf::Color& color, const sf::Vector2f& position) {
    sf::CircleShape circle(radius);
    circle.setFillColor(color);
    circle.setPosition(position - sf::Vector2f(radius, radius));
    return circle;
}

void initializeBody(sf::Vector2f hipPosition, float thighLength, float calfLength) {
    // Set hip position
    hip = createCircle(15, sf::Color::White, hipPosition);

    // Adjust initial angles (in radians)
    float leftThighAngle = M_PI / 2.0 + M_PI / 36.0; // 90 degrees + slight offset
    float rightThighAngle = M_PI / 2.0 - M_PI / 36.0; // 90 degrees - slight offset

    // Calculate knee positions
    sf::Vector2f leftKneePosition = calculatePoint(hipPosition, thighLength, leftThighAngle);
    sf::Vector2f rightKneePosition = calculatePoint(hipPosition, thighLength, rightThighAngle);

    // Create knee circles
    leftKnee = createCircle(8, sf::Color::Blue, leftKneePosition);
    rightKnee = createCircle(8, sf::Color::Blue, rightKneePosition);

    // Calculate feet positions
    sf::Vector2f leftFeetPosition = calculatePoint(leftKneePosition, calfLength, leftThighAngle);
    sf::Vector2f rightFeetPosition = calculatePoint(rightKneePosition, calfLength, rightThighAngle);

    // Create feet circles
    leftFeet = createCircle(6, sf::Color::Cyan, leftFeetPosition);
    rightFeet = createCircle(6, sf::Color::Cyan, rightFeetPosition);

    sf::Vector2f chestPosition = sf::Vector2f(hipPosition.x, hipPosition.y-50);

    chest = createCircle(10, sf::Color::Cyan, chestPosition);
}


// finding possible knee positions
sf::Vector2f findKneePosition(const sf::Vector2f& hipCenter, const sf::Vector2f& feetCenter,
                               float thighLength, float calfLength, const sf::Vector2f& currentKnee) {
    sf::Vector2f hfVec = feetCenter - hipCenter;
    float hfDistance = calculateDistance(hipCenter, feetCenter);

    if (hfDistance > (thighLength + calfLength) || hfDistance < std::abs(thighLength - calfLength)) {
        //std::cout << "Hip and feet too close to too apart";

        if (whichFeet == 0) {
            // Set hip position
            hip.setPosition(sf::Vector2f((leftFeet.getPosition().x + rightFeet.getPosition().x) / 2, 300));

            
        }

        if (whichFeet == 1) {
            // Set hip position
            hip.setPosition(sf::Vector2f((leftFeet.getPosition().x + rightFeet.getPosition().x) / 2, 300));

            
        }
                
    }

    float d = hfDistance;
    float a = (thighLength * thighLength - calfLength * calfLength + d * d) / (2 * d);
    float h = std::sqrt(thighLength * thighLength - a * a);

    sf::Vector2f midpoint = hipCenter + a * (hfVec / d);
    sf::Vector2f perpendicularVec(-hfVec.y / d, hfVec.x / d);

    sf::Vector2f knee1 = midpoint + h * perpendicularVec;
    sf::Vector2f knee2 = midpoint - h * perpendicularVec;

    float dist1 = calculateDistance(knee1, currentKnee);
    float dist2 = calculateDistance(knee2, currentKnee);

    return (dist1 < dist2) ? knee1 : knee2;
}



#include <cstdlib> // For rand()
#include <ctime>   // For seeding rand()

void drawLines(sf::RenderWindow& window, const sf::CircleShape& chest, const sf::CircleShape& hip, 
               const sf::CircleShape& leftKnee, const sf::CircleShape& rightKnee, 
               const sf::CircleShape& leftFeet, const sf::CircleShape& rightFeet) {
    sf::VertexArray lines(sf::Lines, 8);

    sf::Vector2f hipCenter = hip.getPosition() + sf::Vector2f(hip.getRadius(), hip.getRadius());
    sf::Vector2f chestCenter = chest.getPosition() + sf::Vector2f(chest.getRadius(), chest.getRadius());
    sf::Vector2f leftKneeCenter = leftKnee.getPosition() + sf::Vector2f(leftKnee.getRadius(), leftKnee.getRadius());
    sf::Vector2f rightKneeCenter = rightKnee.getPosition() + sf::Vector2f(rightKnee.getRadius(), rightKnee.getRadius());
    sf::Vector2f leftFeetCenter = leftFeet.getPosition() + sf::Vector2f(leftFeet.getRadius(), leftFeet.getRadius());
    sf::Vector2f rightFeetCenter = rightFeet.getPosition() + sf::Vector2f(rightFeet.getRadius(), rightFeet.getRadius());

    lines[0].position = hipCenter; lines[1].position = leftKneeCenter;
    lines[2].position = hipCenter; lines[3].position = rightKneeCenter;
    lines[4].position = leftKneeCenter; lines[5].position = leftFeetCenter;
    lines[6].position = rightKneeCenter; lines[7].position = rightFeetCenter;

    // Draw leg lines
    window.draw(lines);

    // Define a smoothing factor for control point transitions
const float smoothingFactor = 0.005f;

// Generate new random target positions for the control points
float randomX1 = chestCenter.x - 50.0f + static_cast<float>(rand()) / RAND_MAX * 100.0f; // Random x [-50, 50]
float randomX2 = chestCenter.x - 50.0f + static_cast<float>(rand()) / RAND_MAX * 100.0f; // Random x [-50, 50]
float randomY = (hipCenter.y + chestCenter.y) / 2.0f; // Fixed y-coordinate (arithmetic mean)

sf::Vector2f targetControlPoint1(randomX1, randomY);
sf::Vector2f targetControlPoint2(randomX2, randomY);

// Smoothly interpolate control points
static sf::Vector2f controlPoint1 = targetControlPoint1; // Initialize static control points
static sf::Vector2f controlPoint2 = targetControlPoint2;

controlPoint1 += smoothingFactor * (targetControlPoint1 - controlPoint1);
controlPoint2 += smoothingFactor * (targetControlPoint2 - controlPoint2);

// Generate Bezier curve points
sf::VertexArray bezierCurve(sf::LineStrip, 100); // 100 points for smoothness

for (int i = 0; i < 100; ++i) {
    float t = i / 99.0f; // Normalize t to [0, 1]
    float u = 1 - t;

    // Calculate cubic Bezier curve point
    sf::Vector2f point = 
        u * u * u * chestCenter + 
        3 * u * u * t * controlPoint1 + 
        3 * u * t * t * controlPoint2 + 
        t * t * t * hipCenter;

    bezierCurve[i].position = point;
    bezierCurve[i].color = sf::Color::Cyan; // Optional: Set curve color
}

// Draw the Bezier curve
window.draw(bezierCurve);

}



void handleDragging(sf::RenderWindow& window, sf::CircleShape* draggedObject, sf::Vector2f& offset,
                    const sf::CircleShape& hip, sf::CircleShape& leftKnee, sf::CircleShape& rightKnee,
                    sf::CircleShape& leftFeet, sf::CircleShape& rightFeet,
                    float thighLength, float calfLength) {
    if (draggedObject != nullptr) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        draggedObject->setPosition(mousePos - offset);

        sf::Vector2f hipCenter = hip.getPosition() + sf::Vector2f(hip.getRadius(), hip.getRadius());

        if (draggedObject == &leftFeet) {
            whichFeet = 0;
            sf::Vector2f feetCenter = leftFeet.getPosition() + sf::Vector2f(leftFeet.getRadius(), leftFeet.getRadius());
            sf::Vector2f currentKnee = leftKnee.getPosition() + sf::Vector2f(leftKnee.getRadius(), leftKnee.getRadius());
            sf::Vector2f newKnee = findKneePosition(hipCenter, feetCenter, thighLength, calfLength, currentKnee);
            leftKnee.setPosition(newKnee - sf::Vector2f(leftKnee.getRadius(), leftKnee.getRadius()));
        } else if (draggedObject == &rightFeet) {
            whichFeet = 1;
            sf::Vector2f feetCenter = rightFeet.getPosition() + sf::Vector2f(rightFeet.getRadius(), rightFeet.getRadius());
            sf::Vector2f currentKnee = rightKnee.getPosition() + sf::Vector2f(rightKnee.getRadius(), rightKnee.getRadius());
            sf::Vector2f newKnee = findKneePosition(hipCenter, feetCenter, thighLength, calfLength, currentKnee);
            rightKnee.setPosition(newKnee - sf::Vector2f(rightKnee.getRadius(), rightKnee.getRadius()));
        }
    }
}

void handleKeyboardMovement(float moveStep, float thighLength, float calfLength) {
    sf::Vector2f hipCenter = hip.getPosition() + sf::Vector2f(hip.getRadius(), hip.getRadius());
    sf::CircleShape* activeFeet = (whichFeet == 0) ? &leftFeet : &rightFeet;
    sf::CircleShape* activeKnee = (whichFeet == 0) ? &leftKnee : &rightKnee;

    sf::Vector2f feetCenter = activeFeet->getPosition() + sf::Vector2f(activeFeet->getRadius(), activeFeet->getRadius());
    feetCenter.x += moveStep;

    sf::Vector2f currentKnee = activeKnee->getPosition() + sf::Vector2f(activeKnee->getRadius(), activeKnee->getRadius());
    sf::Vector2f newKnee = findKneePosition(hipCenter, feetCenter, thighLength, calfLength, currentKnee);

    activeFeet->setPosition(feetCenter - sf::Vector2f(activeFeet->getRadius(), activeFeet->getRadius()));
    activeKnee->setPosition(newKnee - sf::Vector2f(activeKnee->getRadius(), activeKnee->getRadius()));
    //whichFeet = 1 - whichFeet; // Alternate feet
}

int main() {

    // loopCounter++; // to count loops
    // int loopy = loopCounter%onesteponefoot;
    // if (loopy!=loopCounter){
    //     whichFeet = 1-whichFeet;
    // }

    sf::RenderWindow window(sf::VideoMode(800, 800), "Simple SFML Window");
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.setPosition(sf::Vector2i((desktop.width - window.getSize().x) / 2, (desktop.height - window.getSize().y) / 2));

    // Define hip position, thigh length, and calf length
    sf::Vector2f hipPosition = {400, 300};
    float thighLength = 50.0f; // Length of the thigh
    float calfLength = 60.0f;  // Length of the calf

    // Initialize body parts
    initializeBody(hipPosition, thighLength, calfLength);

    sf::CircleShape* draggedObject = nullptr;
    sf::Vector2f offset;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (controlForm == "Mouse" && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (hip.getGlobalBounds().contains(mousePos)) draggedObject = &hip, offset = mousePos - hip.getPosition();
                else if (leftFeet.getGlobalBounds().contains(mousePos)) draggedObject = &leftFeet, offset = mousePos - leftFeet.getPosition();
                else if (rightFeet.getGlobalBounds().contains(mousePos)) draggedObject = &rightFeet, offset = mousePos - rightFeet.getPosition();
            }
            if (controlForm == "Mouse" && event.type == sf::Event::MouseButtonReleased) {
                draggedObject = nullptr;
            }
        }

        loopCounter++;
        if (loopCounter % onesteponefoot == 0) {
            whichFeet = 1 - whichFeet; // Alternate feet every `onesteponefoot` loops
        }

        if (controlForm == "Key") {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) handleKeyboardMovement(-2.f, thighLength, calfLength);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) handleKeyboardMovement(2.f, thighLength, calfLength);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
                chest.setPosition(sf::Vector2f(chest.getPosition().x+1,chest.getPosition().y-1));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
                chest.setPosition(sf::Vector2f(chest.getPosition().x+1,chest.getPosition().y+1));
            }
             if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
                chest.setPosition(sf::Vector2f(chest.getPosition().x-1,chest.getPosition().y-1));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
                chest.setPosition(sf::Vector2f(chest.getPosition().x-1,chest.getPosition().y+1));
            }
            usleep(15000);
        }

        if (controlForm == "Mouse") {
            handleDragging(window, draggedObject, offset, hip, leftKnee, rightKnee, leftFeet, rightFeet, thighLength, calfLength);
        }

        // Add randomness to chest position
       // Define a smoothing factor (0.0 to 1.0, closer to 1.0 for faster transitions)
const float smoothingFactor = 0.01f;

// Calculate the target position with randomness
float randomX = ((rand() % 42) - 20); // Random x offset in range [-20, 20]
float randomY = ((rand() % 42) - 20); // Random y offset in range [-20, 20]
sf::Vector2f targetChestPosition(
    (leftFeet.getPosition().x + rightFeet.getPosition().x) / 2 + randomX,
    250 + randomY
);

// Get the current position of the chest
sf::Vector2f currentChestPosition = chest.getPosition();

// Interpolate towards the target position
sf::Vector2f smoothChestPosition = currentChestPosition + smoothingFactor * (targetChestPosition - currentChestPosition);

// Update the chest position
chest.setPosition(smoothChestPosition);


        window.clear();
        drawLines(window, chest, hip, leftKnee, rightKnee, leftFeet, rightFeet);
        window.draw(hip);
        window.draw(leftKnee);
        window.draw(rightKnee);
        window.draw(leftFeet);
        window.draw(rightFeet);


        window.draw(chest);
        window.display();
    }

    return 0;
}
