#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <string>
#include <functional>
#include <memory>
#include <iostream>
#include <cmath>
#include <queue>


enum class Operation { None, Insert, Remove, Search, Update, InsertAtHead, InsertAtTail, InsertAtAnyPosition,
                        Push, Pop, Peek, Enqueue, Dequeue, Front, Inorder, Preorder, Postorder};

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    std::function<void()> action;
    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color pressedColor;
    bool isHovered;
    bool isPressed;

public:
    Button(const std::string& label, sf::Font& font, 
            sf::Vector2f position, sf::Vector2f size,
            sf::Color fillColor = sf::Color(0, 0, 0, 150),
            sf::Color textColor = sf::Color::White)
        : text(label, font, 20), 
            normalColor(fillColor),
            hoverColor(sf::Color(fillColor.r, fillColor.g, fillColor.b, 200)),
            pressedColor(sf::Color(fillColor.r, fillColor.g, fillColor.b, 255)),
            isHovered(false),
            isPressed(false) {
        
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(normalColor);
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color::White);
        
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(20);
        text.setFillColor(textColor);
        
        // Center text
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f,
                        textRect.top + textRect.height/2.0f);
        text.setPosition(position.x + size.x/2.0f, 
                        position.y + size.y/2.0f);
    }

    void setAlpha(int alpha) {
        normalColor.a = alpha;
        hoverColor.a = alpha + 50 > 255 ? 255 : alpha + 50;
        pressedColor.a = alpha + 100 > 255 ? 255 : alpha + 100;
    }

    void draw(sf::RenderWindow& window) {
        // Update button state colors
        if (isPressed) {
            shape.setFillColor(pressedColor);
        } else if (isHovered) {
            shape.setFillColor(hoverColor);
        } else {
            shape.setFillColor(normalColor);
        }
        
        window.draw(shape);
        window.draw(text);
    }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        
        isHovered = shape.getGlobalBounds().contains(mousePos);
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left && isHovered) {
                isPressed = true;
            }
        }
        
        if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left && isPressed && isHovered) {
                onClick();
            }
            isPressed = false;
        }
    }

    void onClick() {
        if (action) {
            action();
        }
    }

    void setAction(std::function<void()> newAction) {
        action = newAction;
    }
};
    
class StartScreen {
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text titleText;
    sf::RectangleShape startButton;
    sf::Text startButtonText;
    sf::RectangleShape exitButton;
    sf::Text exitButtonText;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    bool startClicked;

public:
    StartScreen() : startClicked(false) {
        window.create(sf::VideoMode(1200, 672), "Data Structure Visualizer");
        
        // Load font
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load font!" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Load background texture
        if (!backgroundTexture.loadFromFile("background-img.jpg")) {
            // Fallback if image not found
            sf::Image fallbackImage;
            fallbackImage.create(1200, 672, sf::Color(70, 130, 180));
            backgroundTexture.loadFromImage(fallbackImage);
        }

        // Set up background sprite
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
        );

        // Set up title text
        titleText.setFont(font);
        titleText.setString("Data Structure Visualizer");
        titleText.setCharacterSize(60);
        titleText.setFillColor(sf::Color::White);
        titleText.setStyle(sf::Text::Bold);
        sf::FloatRect textBounds = titleText.getLocalBounds();
        titleText.setOrigin(textBounds.width/2, textBounds.height/2);
        titleText.setPosition(window.getSize().x/2, 150);

        // Set up start button
        startButton.setSize(sf::Vector2f(250, 60));
        startButton.setFillColor(sf::Color(0, 0, 0, 150));
        startButton.setOutlineThickness(2);
        startButton.setOutlineColor(sf::Color::White);
        startButton.setPosition(window.getSize().x/2 - 125, 300);

        startButtonText.setFont(font);
        startButtonText.setString("Start");
        startButtonText.setCharacterSize(35);
        startButtonText.setFillColor(sf::Color::White);
        sf::FloatRect startTextBounds = startButtonText.getLocalBounds();
        startButtonText.setOrigin(startTextBounds.width/2, startTextBounds.height/2);
        startButtonText.setPosition(window.getSize().x/2, 320);

        // Set up exit button
        exitButton.setSize(sf::Vector2f(250, 60));
        exitButton.setFillColor(sf::Color(0, 0, 0, 150));
        exitButton.setOutlineThickness(2);
        exitButton.setOutlineColor(sf::Color::White);
        exitButton.setPosition(window.getSize().x/2 - 125, 400);

        exitButtonText.setFont(font);
        exitButtonText.setString("Exit");
        exitButtonText.setCharacterSize(35);
        exitButtonText.setFillColor(sf::Color::White);
        sf::FloatRect exitTextBounds = exitButtonText.getLocalBounds();
        exitButtonText.setOrigin(exitTextBounds.width/2, exitTextBounds.height/2);
        exitButtonText.setPosition(window.getSize().x/2, 420);
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            render();
        }
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect startButtonBounds = startButton.getGlobalBounds();
                    sf::FloatRect exitButtonBounds = exitButton.getGlobalBounds();
                    
                    if (startButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                        startClicked = true;
                        window.close();
                    }

                    if (exitButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                        window.close();
                    }
                }
            }
        }
    }

    void render() {
        window.clear();
        window.draw(backgroundSprite);
        window.draw(titleText);
        window.draw(startButton);
        window.draw(startButtonText);
        window.draw(exitButton);
        window.draw(exitButtonText);
        window.display();
    }

    bool isStartClicked() const { return startClicked; }
};
    
class DataStructureSelectionScreen {
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text titleText;
    sf::RectangleShape arrayButton;
    sf::Text arrayButtonText;
    sf::RectangleShape stringButton;
    sf::Text stringButtonText;
    sf::RectangleShape linkedlistButton;
    sf::Text linkedlistButtonText;
    sf::RectangleShape stackButton;
    sf::Text stackButtonText;
    sf::RectangleShape queueButton;
    sf::Text queueButtonText;
    sf::RectangleShape binarytreeButton;
    sf::Text binarytreeButtonText;
    sf::RectangleShape backButton;
    sf::Text backButtonText;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    std::string selectedStructure;

public:
    DataStructureSelectionScreen() : selectedStructure("") {
        window.create(sf::VideoMode(1200, 672), "Select Data Structure");
        
        // Load font
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load font!" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Load background texture (same as StartScreen)
        if (!backgroundTexture.loadFromFile("background-img.jpg")) {
            // Fallback if image not found
            sf::Image fallbackImage;
            fallbackImage.create(1200, 672, sf::Color(70, 130, 180));
            backgroundTexture.loadFromImage(fallbackImage);
        }

        // Set up background sprite
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
        );

        // Set up title text
        titleText.setFont(font);
        titleText.setString("Select Data Structure");
        titleText.setCharacterSize(60);
        titleText.setFillColor(sf::Color::White);
        titleText.setStyle(sf::Text::Bold);
        sf::FloatRect textBounds = titleText.getLocalBounds();
        titleText.setOrigin(textBounds.width/2, textBounds.height/2);
        titleText.setPosition(window.getSize().x/2, 50);

        // Set up array button
        arrayButton.setSize(sf::Vector2f(350, 80));
        arrayButton.setFillColor(sf::Color(0, 100, 0, 200));
        arrayButton.setOutlineThickness(2);
        arrayButton.setOutlineColor(sf::Color::White);
        arrayButton.setPosition(window.getSize().x/2 - 150, 150);

        arrayButtonText.setFont(font);
        arrayButtonText.setString("Array Visualizer");
        arrayButtonText.setCharacterSize(30);
        arrayButtonText.setFillColor(sf::Color::White);
        sf::FloatRect arrayTextBounds = arrayButtonText.getLocalBounds();
        arrayButtonText.setOrigin(arrayTextBounds.width/2, arrayTextBounds.height/2);
        arrayButtonText.setPosition(window.getSize().x/2 + 25, 190);

        // Set up string button
        stringButton.setSize(sf::Vector2f(350, 80));
        stringButton.setFillColor(sf::Color(0, 0, 100, 200));
        stringButton.setOutlineThickness(2);
        stringButton.setOutlineColor(sf::Color::White);
        stringButton.setPosition(window.getSize().x/2 - 150, 250);

        stringButtonText.setFont(font);
        stringButtonText.setString("String Visualizer");
        stringButtonText.setCharacterSize(30);
        stringButtonText.setFillColor(sf::Color::White);
        sf::FloatRect stringTextBounds = stringButtonText.getLocalBounds();
        stringButtonText.setOrigin(stringTextBounds.width/2, stringTextBounds.height/2);
        stringButtonText.setPosition(window.getSize().x/2 + 25, 290);


        // Set up linked list button
        linkedlistButton.setSize(sf::Vector2f(350, 80));
        linkedlistButton.setFillColor(sf::Color(0, 0, 100, 200));
        linkedlistButton.setOutlineThickness(2);
        linkedlistButton.setOutlineColor(sf::Color::White);
        linkedlistButton.setPosition(window.getSize().x/2 - 450, 350);

        linkedlistButtonText.setFont(font);
        linkedlistButtonText.setString("Linked-List Visualizer");
        linkedlistButtonText.setCharacterSize(30);
        linkedlistButtonText.setFillColor(sf::Color::White);
        sf::FloatRect linkedlistTextBounds = linkedlistButtonText.getLocalBounds();
        linkedlistButtonText.setOrigin(stringTextBounds.width/2, stringTextBounds.height/2);
        linkedlistButtonText.setPosition(window.getSize().x/2 - 315, 385);

        // Set up binary tree button
        binarytreeButton.setSize(sf::Vector2f(350, 80));
        binarytreeButton.setFillColor(sf::Color(0, 0, 100, 200));
        binarytreeButton.setOutlineThickness(2);
        binarytreeButton.setOutlineColor(sf::Color::White);
        binarytreeButton.setPosition(window.getSize().x/2 - 450, 450);

        binarytreeButtonText.setFont(font);
        binarytreeButtonText.setString("Binary Tree Visualizer");
        binarytreeButtonText.setCharacterSize(30);
        binarytreeButtonText.setFillColor(sf::Color::White);
        sf::FloatRect binarytreeTextBounds = binarytreeButtonText.getLocalBounds();
        binarytreeButtonText.setOrigin(stringTextBounds.width/2, stringTextBounds.height/2);
        binarytreeButtonText.setPosition(window.getSize().x/2 - 315, 485);

        // Set up stack button
        stackButton.setSize(sf::Vector2f(350, 80));
        stackButton.setFillColor(sf::Color(0, 0, 100, 200));
        stackButton.setOutlineThickness(2);
        stackButton.setOutlineColor(sf::Color::White);
        stackButton.setPosition(window.getSize().x/2 + 150, 350);

        stackButtonText.setFont(font);
        stackButtonText.setString("Stack Visualizer");
        stackButtonText.setCharacterSize(30);
        stackButtonText.setFillColor(sf::Color::White);
        sf::FloatRect stackTextBounds = stackButtonText.getLocalBounds();
        stackButtonText.setOrigin(stringTextBounds.width/2, stringTextBounds.height/2);
        stackButtonText.setPosition(window.getSize().x/2 + 325, 385);

        // Set up queue button
        queueButton.setSize(sf::Vector2f(350, 80));
        queueButton.setFillColor(sf::Color(0, 0, 100, 200));
        queueButton.setOutlineThickness(2);
        queueButton.setOutlineColor(sf::Color::White);
        queueButton.setPosition(window.getSize().x/2 + 150, 450);

        queueButtonText.setFont(font);
        queueButtonText.setString("Queue Visualizer");
        queueButtonText.setCharacterSize(30);
        queueButtonText.setFillColor(sf::Color::White);
        sf::FloatRect queueTextBounds = queueButtonText.getLocalBounds();
        queueButtonText.setOrigin(stringTextBounds.width/2, stringTextBounds.height/2);
        queueButtonText.setPosition(window.getSize().x/2 + 325, 485);

        // Set up back button
        backButton.setSize(sf::Vector2f(350, 80));
        backButton.setFillColor(sf::Color(150, 0, 0, 200));
        backButton.setOutlineThickness(2);
        backButton.setOutlineColor(sf::Color::White);
        backButton.setPosition(window.getSize().x/2 - 150, 550);

        backButtonText.setFont(font);
        backButtonText.setString("Back to Menu");
        backButtonText.setCharacterSize(30);
        backButtonText.setFillColor(sf::Color::White);
        sf::FloatRect backTextBounds = backButtonText.getLocalBounds();
        backButtonText.setOrigin(backTextBounds.width/2, backTextBounds.height/2);
        backButtonText.setPosition(window.getSize().x/2 + 25, 585);
    }

    std::string run() {
        while (window.isOpen()) {
            handleEvents();
            render();
            
            if (!selectedStructure.empty()) {
                window.close();
                return selectedStructure;
            }
        }
        return "";
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect arrayButtonBounds = arrayButton.getGlobalBounds();
                    sf::FloatRect stringButtonBounds = stringButton.getGlobalBounds();
                    sf::FloatRect linkedlistButtonBounds = linkedlistButton.getGlobalBounds();
                    sf::FloatRect binarytreeButtonBounds = binarytreeButton.getGlobalBounds();
                    sf::FloatRect stackButtonBounds = stackButton.getGlobalBounds();
                    sf::FloatRect queueButtonBounds = queueButton.getGlobalBounds();
                    sf::FloatRect backButtonBounds = backButton.getGlobalBounds();
                    
                    if (arrayButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                        selectedStructure = "array";
                    }
                    else if (stringButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                        selectedStructure = "string";
                    }
                    else if (linkedlistButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                        selectedStructure = "linked-list";
                    }
                    else if (stackButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                        selectedStructure = "stack";
                    }
                    else if (queueButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                        selectedStructure = "queue";
                    }
                    else if (binarytreeButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                        selectedStructure = "binary-tree";
                    }
                    else if (backButtonBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                        selectedStructure = "back";
                    }
                    
                }
            }
        }
    }

    void render() {
        window.clear();
        window.draw(backgroundSprite);
        window.draw(titleText);
        window.draw(arrayButton);
        window.draw(arrayButtonText);
        window.draw(stringButton);
        window.draw(stringButtonText);
        window.draw(linkedlistButton);
        window.draw(linkedlistButtonText);
        window.draw(binarytreeButton);
        window.draw(binarytreeButtonText);
        window.draw(stackButton);
        window.draw(stackButtonText);
        window.draw(queueButton);
        window.draw(queueButtonText);
        window.draw(backButton);
        window.draw(backButtonText);
        window.display();
    }
};
       
class TextInput {
private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Text label;
    sf::Font& font;
    std::string inputString;
    bool isActive;
    int maxLength;

public:
    TextInput(sf::Font& fontRef, sf::Vector2f position, sf::Vector2f size, 
                const std::string& labelText, int maxLen = 10)
        : font(fontRef), isActive(false), maxLength(maxLen) {
        
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::White);
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color(150, 150, 150));
        
        label.setFont(font);
        label.setString(labelText);
        label.setCharacterSize(20);
        label.setFillColor(sf::Color::White);
        label.setPosition(position.x, position.y - 30);
        
        text.setFont(font);
        text.setString("");
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Black);
        text.setPosition(position.x + 10, position.y + (size.y - 24) / 2);
    }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                isActive = shape.getGlobalBounds().contains(mousePos);
                if (isActive) {
                    shape.setOutlineColor(sf::Color::Blue);
                } else {
                    shape.setOutlineColor(sf::Color(150, 150, 150));
                }
            }
        }
        
        if (isActive && event.type == sf::Event::TextEntered) {
            if (event.text.unicode == '\b') { // Backspace
                if (!inputString.empty()) {
                    inputString.pop_back();
                }
            }
            else if (event.text.unicode < 128 && inputString.size() < maxLength) {
                // Allow letters, digits, and spaces
                if (isalpha(static_cast<char>(event.text.unicode)) || 
                isdigit(static_cast<char>(event.text.unicode)) ||
                event.text.unicode == ' ') {
                    inputString += static_cast<char>(event.text.unicode);
                }
            }
            text.setString(inputString);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(label);
        window.draw(shape);
        window.draw(text);
    }

    std::string getText() const { return inputString; }
    void clear() { inputString.clear(); text.setString(inputString); }
    bool isEmpty() const { return inputString.empty(); }
};
    
class ArrayVisualizer {
private:
    std::vector<int> array;
    sf::Vector2f position;
    float cellWidth;
    float cellHeight;
    sf::Font& font;
    sf::Color defaultColor;
    sf::Color highlightColor;

    std::vector<sf::Vector2f> currentPositions; // Actual drawn positions
    std::vector<sf::Vector2f> cellPositions;    //  Target position

    std::vector<size_t> highlightedIndices;

    sf::Color insertingColor;    // Color for insertion
    sf::Color removingColor;     // Color for removal
    sf::Color searchingColor;    // Color for cells being searched
    sf::Color foundColor;       // Color for found element
    std::vector<float> foundBounce;  // For the found element bounce effect

    Operation currentOperation = Operation::None;

    // Variables for update animation
    bool updateAnimating = false;
    size_t updateIndex = 0;
    int oldValue = 0;
    int newValue = 0;
    float updateAnimationProgress = 0.f;
    sf::Vector2f oldCellPosition;
    sf::Vector2f newCellPosition;

public:
    float animationSpeed = 0.03f;

public:
    ArrayVisualizer(sf::Font& fontRef, sf::Vector2f pos, 
                    float width , float height)
        : font(fontRef), position(pos), cellWidth(width), cellHeight(height),
        defaultColor(sf::Color(70, 130, 180)), // Steel blue
        highlightColor(sf::Color(255, 165, 0)),  // Orange
        searchingColor(sf::Color(100, 100, 255)), // Light blue (searching)
        foundColor(sf::Color(0, 255, 0)),         // Bright green (found)
        insertingColor(sf::Color(255, 255, 0, 175)),   // Yellow for insertion
        removingColor(sf::Color(255, 215, 0, 175))     // Gold for removal
    {
        // Initialize with some sample data
        array = {10, 20, 30, 40, 50};
        updateCellPositions();
        // Initialize current positions to match cell positions
        currentPositions = cellPositions;
    }
    
    const std::vector<int>& getArray() const { return array; }

    void draw(sf::RenderWindow& window) {
        for (size_t i = 0; i < array.size(); ++i) {
            // Skip drawing the original cell if it's the one being updated and we're animating
            if (updateAnimating && i == updateIndex) {
                continue;
            }
            
            float x = currentPositions[i].x;
            float y = currentPositions[i].y;
            
            // Apply bounce effect if this is the found element
            float bounceOffset = 0.0f;
            if (i < foundBounce.size() && foundBounce[i] > 0) {
                bounceOffset = -30.0f * foundBounce[i] * foundBounce[i];
            }
            
            sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
            cell.setPosition(x, y + bounceOffset);
            
            // Determine cell color
            if (std::find(highlightedIndices.begin(), highlightedIndices.end(), i) != highlightedIndices.end()) {
                if (i < foundBounce.size() && foundBounce[i] > 0) {
                    cell.setFillColor(foundColor);
                } else if (currentOperation == Operation::Insert) {
                    cell.setFillColor(insertingColor);
                } else if (currentOperation == Operation::Remove) {
                    cell.setFillColor(removingColor);
                } else {
                    cell.setFillColor(searchingColor);
                }
            } else {
                cell.setFillColor(defaultColor);
            }
            
            cell.setOutlineThickness(2.f);
            cell.setOutlineColor(sf::Color::Black);
            window.draw(cell);
            
            // Draw text (position adjusted for bounce)
            sf::Text indexText(std::to_string(i), font, 16);
            indexText.setPosition(x + cellWidth/2 - indexText.getLocalBounds().width/2, 
                                y + cellHeight + 5 + bounceOffset);
            indexText.setFillColor(sf::Color::White);
            window.draw(indexText);
            
            sf::Text valueText(std::to_string(array[i]), font, 20);
            valueText.setPosition(x + cellWidth/2 - valueText.getLocalBounds().width/2, 
                                y + cellHeight/2 - valueText.getLocalBounds().height/2 + bounceOffset);
            valueText.setFillColor(sf::Color::White);
            window.draw(valueText);
        }
        
        // Draw the updating cells if we're in an update animation
        if (updateAnimating) {
            // Draw the old cell sliding down (with old value)
            sf::RectangleShape oldCell(sf::Vector2f(cellWidth, cellHeight));
            oldCell.setPosition(oldCellPosition);
            oldCell.setFillColor(removingColor);
            oldCell.setOutlineThickness(2.f);
            oldCell.setOutlineColor(sf::Color::Black);
            window.draw(oldCell);
            
            sf::Text oldValueText(std::to_string(oldValue), font, 20);
            oldValueText.setPosition(oldCellPosition.x + cellWidth/2 - oldValueText.getLocalBounds().width/2, 
                                   oldCellPosition.y + cellHeight/2 - oldValueText.getLocalBounds().height/2);
            oldValueText.setFillColor(sf::Color::White);
            window.draw(oldValueText);
            
            // Draw the new cell coming down (with new value)
            sf::RectangleShape newCell(sf::Vector2f(cellWidth, cellHeight));
            newCell.setPosition(newCellPosition);
            newCell.setFillColor(insertingColor);
            newCell.setOutlineThickness(2.f);
            newCell.setOutlineColor(sf::Color::Black);
            window.draw(newCell);
            
            sf::Text newValueText(std::to_string(newValue), font, 20);
            newValueText.setPosition(newCellPosition.x + cellWidth/2 - newValueText.getLocalBounds().width/2, 
                                   newCellPosition.y + cellHeight/2 - newValueText.getLocalBounds().height/2);
            newValueText.setFillColor(sf::Color::White);
            window.draw(newValueText);
            
            // Draw index for both cells
            sf::Text indexText(std::to_string(updateIndex), font, 16);
            indexText.setPosition(oldCellPosition.x + cellWidth/2 - indexText.getLocalBounds().width/2, 
                                oldCellPosition.y + cellHeight + 5);
            indexText.setFillColor(sf::Color::White);
            window.draw(indexText);
        }
    }

    void updateCellPositions() {
        cellPositions.clear();
        for (size_t i = 0; i < array.size(); ++i) {
            cellPositions.emplace_back(
                position.x + i * (cellWidth + 5.f),
                position.y
            );
        }
        currentPositions.resize(array.size());
    }


    void insert(int index, int value, bool& animationComplete) {
        currentOperation = Operation::Insert;
        static bool animating = false;
        static size_t insertIndex = 0;
        static std::vector<sf::Vector2f> startPositions;
        
        if (!animating) {
            // Start insertion
            if (index >= 0 && index <= array.size()) {
                array.insert(array.begin() + index, value);
                updateCellPositions();
                
                // Set start positions for animation
                startPositions = cellPositions;
                for (size_t i = 0; i < array.size(); i++) {
                    if (i == index) {
                        startPositions[i] = sf::Vector2f(cellPositions[i].x, cellPositions[i].y - 100);
                    } else if (i > index) {
                        startPositions[i] = sf::Vector2f(cellPositions[i].x - (cellWidth + 5), cellPositions[i].y);
                    }
                }
                currentPositions = startPositions;
                insertIndex = index;
                animating = true;
                animationComplete = false;
                
                // Highlight cells that will move (the inserted cell and all to its right)
                highlightedIndices.clear();
                for (size_t i = index; i < array.size(); ++i) {
                    highlightedIndices.push_back(i);
                }
            }
        } else {
            // Animate insertion
            bool allComplete = true;
            for (size_t i = 0; i < array.size(); ++i) {
                if (i >= insertIndex) {
                    currentPositions[i] = currentPositions[i] + (cellPositions[i] - currentPositions[i]) * animationSpeed;
                    
                    float dx = cellPositions[i].x - currentPositions[i].x;
                    float dy = cellPositions[i].y - currentPositions[i].y;
                    float distanceSquared = dx*dx + dy*dy;
                    
                    if (distanceSquared >= 1.0f) {
                        allComplete = false;
                    } else {
                        currentPositions[i] = cellPositions[i];
                    }
                }
            }
            
            if (allComplete) {
                animating = false;
                animationComplete = true;
                highlightedIndices.clear(); // Clear highlights when done
            }
        }
    }


    void remove(int index, bool& animationComplete) {
        currentOperation = Operation::Remove;
        static bool animating = false;
        static size_t removeIndex = 0;
        static std::vector<sf::Vector2f> startPositions;
        static std::vector<sf::Vector2f> targetPositions;
        static float animationProgress = 0.f;
        static int removedValue = 0;
        
        const float shiftingSpeed = 0.03f;  
    
        if (!animating) {
            // Start removal
            if (index >= 0 && index < array.size()) {
                removedValue = array[index];
                removeIndex = index;
                
                // Store current positions
                startPositions = currentPositions;
                
                // Calculate target positions (shift left for right neighbors)
                targetPositions = startPositions;
                targetPositions.erase(targetPositions.begin() + index);
                
                for (size_t i = index; i < targetPositions.size(); ++i) {
                    targetPositions[i].x = startPositions[i+1].x - (cellWidth + 5);
                }
                
                animating = true;
                animationComplete = false;
                
                // Highlight cells that will move (the removed cell and all to its right)
                highlightedIndices.clear();
                highlightedIndices.push_back(index); // The cell being removed
                for (size_t i = index + 1; i < array.size(); ++i) {
                    highlightedIndices.push_back(i); // Cells shifting left
                }
            }
        } else {
            // Animate removal
            animationProgress += shiftingSpeed;
            
            // 1. Animate the removed cell sliding down
            if (removeIndex < currentPositions.size()) {
                currentPositions[removeIndex].y = startPositions[removeIndex].y + 
                    (80.f * animationProgress);
            }
            
            // 2. Animate neighboring cells shifting left
            for (size_t i = 0; i < currentPositions.size(); ++i) {
                if (i != removeIndex && i >= removeIndex) {
                    currentPositions[i].x = startPositions[i].x + 
                        (targetPositions[i-1].x - startPositions[i].x) * animationProgress;
                }
            }
            
            // Check if animation is complete
            if (animationProgress >= 1.0f) {
                // Actually remove the element
                array.erase(array.begin() + removeIndex);
                updateCellPositions();
                currentPositions = cellPositions;
                
                animating = false;
                animationComplete = true;
                highlightedIndices.clear(); // Clear highlights when done
            }
        }
        
    }

    void search(int value, bool& animationComplete) {
        currentOperation = Operation::Search;
        static bool animating = false;
        static size_t currentIndex = 0;
        static int foundIndex = -1;
        static std::vector<float> searchProgress;
        static float foundHoldTime = 0.0f;
        static const float HOLD_DURATION = 2.0f;
        
        if (!animating) {
            // Reset positions and states when starting new search
            for (size_t i = 0; i < currentPositions.size(); ++i) {
                currentPositions[i] = cellPositions[i];
            }
            
            // Initialize search state
            foundIndex = -1;
            currentIndex = 0;
            foundHoldTime = 0.0f;
            searchProgress.assign(array.size(), 0.0f);
            foundBounce.assign(array.size(), 0.0f); // Clear all bounce states
            animating = true;
            animationComplete = false;
            highlightedIndices.clear();
        } else {
            // If we've found an element and are in the hold phase
            if (foundIndex != -1 && foundHoldTime < HOLD_DURATION) {
                foundHoldTime += 1.0f / 60.0f;
                
                // Keep the element elevated
                if (foundIndex < currentPositions.size()) {
                    currentPositions[foundIndex].y = cellPositions[foundIndex].y - 5.0f;
                }
                
                // Highlight only the found element
                highlightedIndices.clear();
                highlightedIndices.push_back(foundIndex);
                
                // Ensure foundBounce is properly set for the found element
                if (foundIndex < foundBounce.size()) {
                    foundBounce[foundIndex] = 1.0f; // Maintain bounce state during hold
                }
                
                if (foundHoldTime >= HOLD_DURATION) {
                    // Reset positions and states
                    for (size_t i = 0; i < currentPositions.size(); ++i) {
                        currentPositions[i] = cellPositions[i];
                    }
                    highlightedIndices.clear();
                    foundBounce.assign(array.size(), 0.0f); // Clear all bounce states
                    animationComplete = true;
                    animating = false;
                }
                return;
            }
            
            // Animate search
            if (currentIndex < array.size()) {
                highlightedIndices.clear();
                highlightedIndices.push_back(currentIndex);
                
                searchProgress[currentIndex] += animationSpeed;
                
                if (searchProgress[currentIndex] >= 1.0f) {
                    if (array[currentIndex] == value) {
                        foundIndex = currentIndex;
                        // Ensure foundBounce is properly sized
                        if (foundBounce.size() <= foundIndex) {
                            foundBounce.resize(foundIndex + 1, 0.0f);
                        }
                        foundBounce[foundIndex] = 1.0f; // Start bounce
                        currentPositions[foundIndex].y = cellPositions[foundIndex].y - 5.0f;
                        foundHoldTime = 0.0f;
                    } else {
                        currentIndex++;
                    }
                }
            } else {
                // Search complete (element not found)
                animating = false;
                animationComplete = true;
            }
        }
        
        // Update bounce animation for found element
        if (foundIndex != -1 && foundIndex < foundBounce.size() && foundBounce[foundIndex] > 0) {
            foundBounce[foundIndex] -= animationSpeed * 0.5f;
            if (foundBounce[foundIndex] < 0) foundBounce[foundIndex] = 0;
        }
    }

    void updateValue(int index, int value, bool& animationComplete) {
        currentOperation = Operation::Update;
        
        if (!updateAnimating) {
            // Initialize update animation
            if (index >= 0 && index < array.size()) {
                updateIndex = index;
                oldValue = array[index];
                newValue = value;
                updateAnimationProgress = 0.f;
                
                // Set initial positions
                oldCellPosition = cellPositions[index];
                newCellPosition = oldCellPosition - sf::Vector2f(0, 100); // Start above
                
                highlightedIndices.clear();
                highlightedIndices.push_back(index);
                
                updateAnimating = true;
                animationComplete = false;
            }
        } else {
            // Update animation progress
            updateAnimationProgress += animationSpeed * 0.5f;
            
            // Animate both cells simultaneously
            float progress = std::min(updateAnimationProgress, 1.0f);
            
            // Old cell slides down
            oldCellPosition.y = cellPositions[updateIndex].y + 100.0f * progress;
            
            // New cell comes down from above
            newCellPosition.y = cellPositions[updateIndex].y - 100.0f * (1.0f - progress);
            
            // Update actual array value at 50% progress
            if (updateAnimationProgress >= 0.5f && array[updateIndex] != newValue) {
                array[updateIndex] = newValue;
            }
            
            // Complete animation
            if (updateAnimationProgress >= 1.0f) {
                array[updateIndex] = newValue; // Ensure final value
                updateCellPositions();
                currentPositions = cellPositions;
                
                updateAnimating = false;
                animationComplete = true;
                highlightedIndices.clear();
            }
        }
    }

};

class StringVisualizer{
private:    
    std::string text;
    sf::Vector2f position;
    sf::Font& font;
    std::vector<sf::RectangleShape> characterBoxes;
    std::vector<sf::Text> characterTexts;
    std::vector<sf::Text> indexTexts;
    float indexTextOffset = 10.f; 
    float boxWidth, boxHeight;

    std::vector<sf::Vector2f> currentPositions; // Actual drawn positions
    std::vector<sf::Vector2f> cellPositions;   
    
    Operation currentOperation = Operation::None;

    std::vector<size_t> highlightedIndices;

    sf::Color defaultColor;
    sf::Color insertingColor;    // Color for insertion
    sf::Color removingColor;     // Color for removal
    sf::Color searchingColor;    // Color for cells being searched
    sf::Color foundColor;       // Color for found element
    std::vector<float> foundBounce;  // For the found element bounce effect

    // Variables for update animation
    bool updateAnimating = false;
    size_t updateIndex = 0;
    char oldValue;
    char newValue;
    float updateAnimationProgress = 0.f;
    sf::Vector2f oldCellPosition;
    sf::Vector2f newCellPosition;


public:
    float animationSpeed = 0.03f;    

public:
    StringVisualizer(sf::Font& fontRef, sf::Vector2f pos, float width,
                     float height)
        : font(fontRef), position(pos), boxWidth(width), boxHeight(height),
        defaultColor(sf::Color(0, 0, 0)), // black
        // defaultColor(sf::Color(70, 130, 180)), // Changed to visible color
        insertingColor(sf::Color(255, 255, 0, 175)), // Yellow for insertion
        removingColor(sf::Color(255, 215, 0, 175)),     // Gold for removal
        searchingColor(sf::Color(100, 100, 255)), // Light blue (searching)
        foundColor(sf::Color(0, 255, 0))         // Bright green (found)
        {
            text = "Hello";
            updatecellPosition();
            // Initialize current positions to match cell positions
            currentPositions = cellPositions;
        }      

    std::string getString(){
        return text;
    }

    void updatecellPosition(){
        cellPositions.clear();
        for (size_t i = 0; i < text.size(); ++i) {
            cellPositions.emplace_back(
                position.x + (i * boxWidth),
                position.y
            );
        }
        currentPositions.resize(cellPositions.size());
    }

    void draw(sf::RenderWindow& window){

        for(size_t i = 0; i <text.size(); ++i){

            // Skip drawing the original cell if it's the one being updated and we're animating
            if (updateAnimating && i == updateIndex) {
                continue;
            }

            float x = currentPositions[i].x;
            float y = currentPositions[i].y;

            // Apply bounce effect if this is the found element
            float bounceOffset = 0.0f;
            if (i < foundBounce.size() && foundBounce[i] > 0) {
                bounceOffset = -30.0f * foundBounce[i] * foundBounce[i];
            }

            // draw box
            sf::RectangleShape box(sf::Vector2f(boxWidth, boxHeight));
            box.setPosition(x, y + bounceOffset);

            // Determine cell color
            if (std::find(highlightedIndices.begin(), highlightedIndices.end(), i) != highlightedIndices.end()) {
                if (i < foundBounce.size() && foundBounce[i] > 0) {
                    box.setFillColor(foundColor);
                } else if (currentOperation == Operation::Insert) {
                    box.setFillColor(insertingColor);
                } else if (currentOperation == Operation::Remove) {
                    box.setFillColor(removingColor);
                } else {
                    box.setFillColor(searchingColor);
                }
            } else {
                box.setFillColor(defaultColor);
            }

            box.setOutlineThickness(2.f);
            box.setOutlineColor(sf::Color::Black);
            window.draw(box);

            // draw index
            sf::Text indexText(std::to_string(i), font, 16);
            indexText.setPosition(x + boxWidth/2 - indexText.getLocalBounds().width/2, 
                                y + boxHeight + 5 + bounceOffset);
            indexText.setFillColor(sf::Color::White);
            window.draw(indexText);

            // draw value
            sf::Text valueText(std::string(1, text[i]), font, 18);
            valueText.setPosition(x + boxWidth/2 - valueText.getLocalBounds().width/2, 
                                y + boxHeight/2 - valueText.getLocalBounds().height/2 + bounceOffset);
            valueText.setFillColor(sf::Color::White);
            window.draw(valueText);
        }

        // Draw the updating cells if we're in an update animation
        if (updateAnimating) {
            // Draw the old cell sliding down (with old value)
            sf::RectangleShape oldCell(sf::Vector2f(boxWidth, boxHeight));
            oldCell.setPosition(oldCellPosition);
            oldCell.setFillColor(removingColor);
            oldCell.setOutlineThickness(2.f);
            oldCell.setOutlineColor(sf::Color::Black);
            window.draw(oldCell);
            
            sf::Text oldValueText(std::string(1, oldValue), font, 20);
            oldValueText.setPosition(oldCellPosition.x + boxWidth/2 - oldValueText.getLocalBounds().width/2, 
                                   oldCellPosition.y + boxHeight/2 - oldValueText.getLocalBounds().height/2);
            oldValueText.setFillColor(sf::Color::White);
            window.draw(oldValueText);
            
            // Draw the new cell coming down (with new value)
            sf::RectangleShape newCell(sf::Vector2f(boxWidth, boxHeight));
            newCell.setPosition(newCellPosition);
            newCell.setFillColor(insertingColor);
            newCell.setOutlineThickness(2.f);
            newCell.setOutlineColor(sf::Color::Black);
            window.draw(newCell);
            
            sf::Text newValueText(std::string(1, newValue), font, 20);
            newValueText.setPosition(newCellPosition.x + boxWidth/2 - newValueText.getLocalBounds().width/2, 
                                   newCellPosition.y + boxHeight/2 - newValueText.getLocalBounds().height/2);
            newValueText.setFillColor(sf::Color::White);
            window.draw(newValueText);
            
            // Draw index for both cells
            sf::Text indexText(std::to_string(updateIndex), font, 16);
            indexText.setPosition(oldCellPosition.x + boxWidth/2 - indexText.getLocalBounds().width/2, 
                                oldCellPosition.y + boxHeight + 5);
            indexText.setFillColor(sf::Color::White);
            window.draw(indexText);
        }
    }

    void insert(int index,  const std::string& value, bool& animationComplete){

        currentOperation = Operation::Insert;
        static bool animating = false;
        static size_t insertIndex = 0;
        static std::vector<sf::Vector2f> startPositions;

        if(!animating){
            // Start insertion
            if(index >=0 && index <= text.size()){
                text.insert(index, value);
                updatecellPosition();


                // Set start positions for animation
                startPositions = cellPositions;


                // New characters start above
                for(size_t i = index; i < index + value.size(); i++) {
                    startPositions[i] = sf::Vector2f(position.x + i * boxWidth, position.y - 100);
                }
                
                // Existing characters start shifted left
                for(size_t i = index + value.size(); i < text.size(); i++) {
                    startPositions[i] = sf::Vector2f(position.x + (i - value.size()) * boxWidth, position.y);
                }

                currentPositions = startPositions;
                insertIndex = index;
                animating = true;
                animationComplete = false;

                

                // Highlight cells that will move (the inserted cell and all to its right)
                highlightedIndices.clear();
                for (size_t i = index; i < text.size(); ++i) {
                    highlightedIndices.push_back(i);
                }
            } 
        }
        else{
            // Animate insertion
            bool allComplete = true;
            for (size_t i = 0; i < text.size(); ++i) {
                if (i >= insertIndex) {
                    currentPositions[i] = currentPositions[i] + (cellPositions[i] - currentPositions[i]) * animationSpeed;
                    
                    float dx = cellPositions[i].x - currentPositions[i].x;
                    float dy = cellPositions[i].y - currentPositions[i].y;
                    float distanceSquared = dx*dx + dy*dy;
                    
                    if (distanceSquared >= 1.0f) {
                        allComplete = false;
                    } else {
                        currentPositions[i] = cellPositions[i];
                    }
                }
            }
            
            if (allComplete) {
                animating = false;
                animationComplete = true;
                highlightedIndices.clear(); // Clear highlights when done
                currentOperation = Operation::None;
            }
        }
    }

    void remove(int index, bool& animationComplete){
        currentOperation = Operation::Remove;
        static bool animating = false;
        static size_t removeIndex = 0;
        static std::vector<sf::Vector2f> startPositions;
        static std::vector<sf::Vector2f> targetPositions;
        static float animationProgress = 0.f;
        static int removedValue = 0;  
    
        if (!animating) {
            // Start removal
            if (index >= 0 && index < text.size()) {
                removedValue = text[index];
                removeIndex = index;
                
                // Store current positions
                startPositions = currentPositions;
                
                // Calculate target positions (shift left for right neighbors)
                targetPositions = startPositions;
                targetPositions.erase(targetPositions.begin() + index);
                
                for (size_t i = index; i < targetPositions.size(); ++i) {
                    targetPositions[i].x = startPositions[i+1].x - (boxWidth);
                }
                
                animating = true;
                animationComplete = false;
                
                // Highlight cells that will move (the removed cell and all to its right)
                highlightedIndices.clear();
                highlightedIndices.push_back(index); // The cell being removed
                for (size_t i = index + 1; i < text.size(); ++i) {
                    highlightedIndices.push_back(i); // Cells shifting left
                }
            }
        } else {
            // Animate removal
            animationProgress += animationSpeed;
            
            // 1. Animate the removed cell sliding down
            if (removeIndex < currentPositions.size()) {
                currentPositions[removeIndex].y = startPositions[removeIndex].y + 
                    (100.f * animationProgress);
            }
            
            // 2. Animate neighboring cells shifting left
            for (size_t i = 0; i < currentPositions.size(); ++i) {
                if (i != removeIndex && i >= removeIndex) {
                    currentPositions[i].x = startPositions[i].x + 
                        (targetPositions[i-1].x - startPositions[i].x) * animationProgress;
                }
            }
            
            // Check if animation is complete
            if (animationProgress >= 1.0f) {
                // Actually remove the element
                text.erase(removeIndex, 1);
                updatecellPosition();
                currentPositions = cellPositions;
                
                animating = false;
                animationComplete = true;
                highlightedIndices.clear(); // Clear highlights when done
            }
        }
    }

    void updatevalue(int index, char value, bool& animationComplete){
        currentOperation = Operation::Update;

        if (!updateAnimating) {
            // Initialize update animation
            if (index >= 0 && index < text.size()) {
                updateIndex = index;
                oldValue = text[index];
                newValue = value;
                updateAnimationProgress = 0.f;
                
                // Set initial positions
                oldCellPosition = cellPositions[index];
                newCellPosition = oldCellPosition - sf::Vector2f(0, 100); // Start above
                
                highlightedIndices.clear();
                highlightedIndices.push_back(index);
                
                updateAnimating = true;
                animationComplete = false;
            }
            
        }else {
            // Update animation progress
            updateAnimationProgress += animationSpeed * 0.3f;
            
            // Animate both cells simultaneously
            float progress = std::min(updateAnimationProgress, 1.0f);
            
            // Old cell slides down
            oldCellPosition.y = cellPositions[updateIndex].y + 100.0f * progress;
            
            // New cell comes down from above
            newCellPosition.y = cellPositions[updateIndex].y - 100.0f * (1.0f - progress);
            
            //updates value at 50% progress:
            if (updateAnimationProgress >= 0.5f && text[updateIndex] != newValue) {
                text[updateIndex] = newValue;
            }

            // Complete animation
            if (updateAnimationProgress >= 1.0f) {
                text[updateIndex] = newValue; // Ensure final value
                updatecellPosition();
                currentPositions = cellPositions;
                
                updateAnimating = false;
                animationComplete = true;
                highlightedIndices.clear();
            }
        }
    }

    void search(int value, bool& animationComplete) {
        currentOperation = Operation::Search;
        static bool animating = false;
        static size_t currentIndex = 0;
        static int foundIndex = -1;
        static std::vector<float> searchProgress;
        static float foundHoldTime = 0.0f;
        static const float HOLD_DURATION = 2.0f;
        
        if (!animating) {
            // Reset positions and states when starting new search
            for (size_t i = 0; i < currentPositions.size(); ++i) {
                currentPositions[i] = cellPositions[i];
            }
            
            // Initialize search state
            foundIndex = -1;
            currentIndex = 0;
            foundHoldTime = 0.0f;
            searchProgress.assign(text.size(), 0.0f);
            foundBounce.assign(text.size(), 0.0f); // Clear all bounce states
            animating = true;
            animationComplete = false;
            highlightedIndices.clear();

        } else {
            // If we've found an element and are in the hold phase
            if (foundIndex != -1 && foundHoldTime < HOLD_DURATION) {
                foundHoldTime += 1.0f / 60.0f;
                
                // Keep the element elevated
                if (foundIndex < currentPositions.size()) {
                    currentPositions[foundIndex].y = cellPositions[foundIndex].y - 5.0f;
                }
                
                // Highlight only the found element
                highlightedIndices.clear();
                highlightedIndices.push_back(foundIndex);
                
                // Ensure foundBounce is properly set for the found element
                if (foundIndex < foundBounce.size()) {
                    foundBounce[foundIndex] = 1.0f; // Maintain bounce state during hold
                }
                
                if (foundHoldTime >= HOLD_DURATION) {
                    // Reset positions and states
                    for (size_t i = 0; i < currentPositions.size(); ++i) {
                        currentPositions[i] = cellPositions[i];
                    }
                    highlightedIndices.clear();
                    foundBounce.assign(text.size(), 0.0f); // Clear all bounce states
                    animationComplete = true;
                    animating = false;
                }
                return;
            }
            
            // Animate search
            if (currentIndex < text.size()) {
                highlightedIndices.clear();
                highlightedIndices.push_back(currentIndex);
                
                searchProgress[currentIndex] += animationSpeed;
                
                if (searchProgress[currentIndex] >= 1.0f) {
                    if (text[currentIndex] == value) {
                        foundIndex = currentIndex;
                        // Ensure foundBounce is properly sized
                        if (foundBounce.size() <= foundIndex) {
                            foundBounce.resize(foundIndex + 1, 0.0f);
                        }
                        foundBounce[foundIndex] = 1.0f; // Start bounce
                        currentPositions[foundIndex].y = cellPositions[foundIndex].y - 5.0f;
                        foundHoldTime = 0.0f;
                    } else {
                        currentIndex++;
                    }
                }
            } else {
                // Search complete (element not found)
                animating = false;
                animationComplete = true;
            }
        }
        
        // Update bounce animation for found element
        if (foundIndex != -1 && foundIndex < foundBounce.size() && foundBounce[foundIndex] > 0) {
            foundBounce[foundIndex] -= animationSpeed * 0.5f;
            if (foundBounce[foundIndex] < 0) foundBounce[foundIndex] = 0;
        }
    }

};

class Linkedlistvisualizer{
public:
    class Node{
        public:
            int data;
            Node* next;
            sf::Vector2f position;

        Node(int d) : data(d), next(nullptr), position(0,0){}
    };

    Node* head;
    Node* tail;
    int size;

    // Visualization properties
    sf::Vector2f basePosition;
    float nodeWidth;
    float nodeHeight;
    float arrowLength;
    sf::Font& font;

    // Color
    sf::Color defaultColor;
    sf::Color insertingColor;
    sf::Color removingColor;
    sf::Color searchingColor;
    sf::Color foundColor;

    int oldValue;  // For update animation
    int newValue;  // For update animation
    
    // Animation state
    bool isAnimating;
    float animationProgress;
    Node* newNode; // For insertion animation
    sf::Vector2f originalDeletePosition ;
    std::vector<Node*> nodesToMove; // For insertion animation
    std::vector<sf::Vector2f> originalPositions; // For insertion animation

    std::vector<size_t> highlightedIndices;

    std::vector<float> foundBounce;  // For the found element bounce effect

    Operation currentOperation = Operation::None;

public:
    float animationSpeed = 0.03f;    
public:
    Linkedlistvisualizer(sf::Font& fontRef, sf::Vector2f pos, 
        float width, float height)
        : head(nullptr), tail(nullptr), size(0),
        basePosition(pos), nodeWidth(width), nodeHeight(height), 
        arrowLength(50.f), font(fontRef),defaultColor(sf::Color(70, 130, 180)),
        insertingColor(sf::Color(255, 255, 0, 175)),
        removingColor(sf::Color(255, 215, 0, 175)),
        searchingColor(sf::Color(100, 100, 255)),
        foundColor(sf::Color(0, 255, 0)),
        isAnimating(false),
        animationProgress(0.f),
        newNode(nullptr)
        {
            // Initialize with some sample data (without animation)
            Node* node1 = new Node(10);
            Node* node2 = new Node(20);
            Node* node3 = new Node(30);
            Node* node4 = new Node(40);
            
            head = node1;
            node1->next = node2;
            node2->next = node3;
            node3->next = node4;
            tail = node4;
            size = 4;
            updateNodePositions();
        }
    ~Linkedlistvisualizer() {
        clear();
    } 

    void updateNodePositions() {
        Node* current = head;
        int index = 0;
        while (current != nullptr) {
            current->position = sf::Vector2f(
                basePosition.x + index * (nodeWidth + arrowLength),
                basePosition.y
            );
            current = current->next;
            index++;
        }
    }

    void draw(sf::RenderWindow& window) {
        // Draw all nodes and arrows
        Node* current = head;
        int index = 1;  // Track node position (1-based)
        
        while (current != nullptr) {
            // Skip drawing if this is the node being deleted
            if (isAnimating && current == newNode) {
                current = current->next;
                index++;
                continue;
            }
    
            // Calculate bounce offset if this node is found during search
            float bounceOffset = 0.0f;
            if (index < foundBounce.size() && foundBounce[index] > 0) {
                bounceOffset = -30.0f * foundBounce[index] * foundBounce[index];
            }
    
            // Draw node
            sf::RectangleShape nodeRect(sf::Vector2f(nodeWidth, nodeHeight));
            sf::Vector2f nodePosition = current->position;
            nodePosition.y += bounceOffset;  // Apply bounce effect
            nodeRect.setPosition(nodePosition);
            
            // Determine node color based on operation and highlighting
            if (std::find(highlightedIndices.begin(), highlightedIndices.end(), index) != highlightedIndices.end()) {
                if (foundBounce.size() > index && foundBounce[index] > 0) {
                    nodeRect.setFillColor(foundColor);  // Green for found node
                } else if (currentOperation == Operation::Search) {
                    nodeRect.setFillColor(searchingColor);  // Blue for nodes being searched
                } else if (currentOperation == Operation::Insert) {
                    nodeRect.setFillColor(insertingColor);  // Yellow for insertion
                } else if (currentOperation == Operation::Remove) {
                    nodeRect.setFillColor(removingColor);  // Gold for removal
                }
            } else {
                // Default color or special cases
                if (isAnimating && current == tail && newNode != nullptr) {
                    nodeRect.setFillColor(sf::Color(255, 200, 0)); // Gold for tail during insertion
                } else {
                    nodeRect.setFillColor(defaultColor);  // Default steel blue
                }
            }
            
            nodeRect.setOutlineThickness(2.f);
            nodeRect.setOutlineColor(sf::Color::Black);
            window.draw(nodeRect);
            
            // Draw node value (with bounce offset)
            sf::Text valueText(std::to_string(current->data), font, 20);
            valueText.setPosition(
                nodePosition.x + nodeWidth/2 - valueText.getLocalBounds().width/2,
                nodePosition.y + nodeHeight/2 - valueText.getLocalBounds().height/2
            );
            valueText.setFillColor(sf::Color::White);
            window.draw(valueText);
    
            // Draw node index below
            sf::Text indexText(std::to_string(index - 1), font, 16);  // 0-based index
            indexText.setPosition(
                nodePosition.x + nodeWidth/2 - indexText.getLocalBounds().width/2,
                nodePosition.y + nodeHeight + 5
            );
            indexText.setFillColor(sf::Color::White);
            window.draw(indexText);
            
            // Draw arrows (skip if pointing to nullptr or to the node being deleted)
            if (current->next != nullptr && !(isAnimating && current->next == newNode)) {
                // Calculate bounce offset for next node
                float nextBounceOffset = 0.0f;
                if (index+1 < foundBounce.size() && foundBounce[index+1] > 0) {
                    nextBounceOffset = -30.0f * foundBounce[index+1] * foundBounce[index+1];
                }

                // Start position (right side of current node)
                sf::Vector2f startPos = nodePosition + sf::Vector2f(nodeWidth, nodeHeight/2);
                
                // End position (left side of next node)
                sf::Vector2f endPos = current->next->position + sf::Vector2f(0, nodeHeight/2);
                endPos.y += nextBounceOffset; // Apply bounce to next node

                // If this node or next node is found, angle the arrow upward
                bool isFoundNode = (foundBounce.size() > index && foundBounce[index] > 0);
                bool isNextNodeFound = (foundBounce.size() > index+1 && foundBounce[index+1] > 0);

                if (isFoundNode || isNextNodeFound) {
                    // Adjust positions for upward angle
                    startPos.y -= 15;  // Increased from 10 to 15 for more visible angle
                    endPos.y -= 15;
                    
                    // Draw straight arrow at angle
                    sf::Vertex line[] = {
                        sf::Vertex(startPos, sf::Color::White),
                        sf::Vertex(endPos, sf::Color::White)
                    };
                    window.draw(line, 2, sf::Lines);
                    
                    // Larger, more visible upward-pointing arrowhead
                    sf::ConvexShape arrowHead;
                    arrowHead.setPointCount(3);
                    arrowHead.setPoint(0, sf::Vector2f(-8, 8));   // Increased size
                    arrowHead.setPoint(1, sf::Vector2f(8, 8));    // Increased size
                    arrowHead.setPoint(2, sf::Vector2f(0, -8));   // Increased size
                    arrowHead.setFillColor(sf::Color::White);
                    arrowHead.setOutlineThickness(1.f);           // Added outline
                    arrowHead.setOutlineColor(sf::Color::Black);   // Contrast outline
                    arrowHead.setPosition(endPos);
                    window.draw(arrowHead);
                } else {
                    // Normal horizontal arrow
                    sf::Vertex line[] = {
                        sf::Vertex(startPos, sf::Color::White),
                        sf::Vertex(endPos, sf::Color::White)
                    };
                    window.draw(line, 2, sf::Lines);

                    // Larger normal arrow head
                    sf::ConvexShape arrowHead;
                    arrowHead.setPointCount(3);
                    arrowHead.setPoint(0, sf::Vector2f(0, -8));   // Increased size
                    arrowHead.setPoint(1, sf::Vector2f(0, 8));    // Increased size
                    arrowHead.setPoint(2, sf::Vector2f(12, 0));   // Increased size
                    arrowHead.setFillColor(sf::Color::White);
                    arrowHead.setOutlineThickness(1.f);           // Added outline
                    arrowHead.setOutlineColor(sf::Color::Black);  // Contrast outline
                    arrowHead.setPosition(endPos.x - 12, endPos.y); // Adjusted position
                    window.draw(arrowHead);
                }
            }
                
            current = current->next;
            index++;
        }
    
        // Draw the new node if it's being animated (for insertions)
        if (isAnimating && newNode != nullptr && currentOperation != Operation::Remove) {
            sf::RectangleShape newNodeRect(sf::Vector2f(nodeWidth, nodeHeight));
            newNodeRect.setPosition(newNode->position);
            newNodeRect.setFillColor(insertingColor); // Yellow for new node
            newNodeRect.setOutlineThickness(2.f);
            newNodeRect.setOutlineColor(sf::Color::Black);
            window.draw(newNodeRect);
            
            // Draw new node value
            sf::Text valueText(std::to_string(newNode->data), font, 20);
            valueText.setPosition(
                newNode->position.x + nodeWidth/2 - valueText.getLocalBounds().width/2,
                newNode->position.y + nodeHeight/2 - valueText.getLocalBounds().height/2
            );
            valueText.setFillColor(sf::Color::White);
            window.draw(valueText);
        }
    }

    // Core operations
    void insertAtHead(int value, bool& animationComplete) {
        if (!isAnimating) {
            // Start insertion animation
            newNode = new Node(value);
            
            // Store original positions of nodes that will move
            originalPositions.clear();
            nodesToMove.clear();
            
            Node* current = head;
            while (current != nullptr) {
                originalPositions.push_back(current->position);
                nodesToMove.push_back(current);
                current = current->next;
            }
            
            // Set initial position for new node (above the first node or base position)
            if (head != nullptr) {
                newNode->position = head->position - sf::Vector2f(0, 100);
            } else {
                newNode->position = basePosition - sf::Vector2f(0, 100);
            }
            
            isAnimating = true;
            animationProgress = 0.f;
            animationComplete = false;
        } else {
            // Animate insertion
            animationProgress += animationSpeed * 0.5;
            
            // 1. Animate new node coming down
            if (nodesToMove.empty()) {
                newNode->position.y = basePosition.y - 100 * (1 - animationProgress);
            } else {
                newNode->position.y = nodesToMove[0]->position.y - 100 * (1 - animationProgress);
            }
            
            // 2. Animate existing nodes moving right
            for (size_t i = 0; i < nodesToMove.size(); i++) {
                float targetX = basePosition.x + (i + 1) * (nodeWidth + arrowLength);
                nodesToMove[i]->position.x = originalPositions[i].x + 
                                          (targetX - originalPositions[i].x) * animationProgress;
            }
            
            // Check if animation is complete
            if (animationProgress >= 1.0f) {
                // Complete the insertion
                newNode->next = head;
                head = newNode;
                if (tail == nullptr) {
                    tail = newNode;
                }
                size++;
                
                isAnimating = false;
                animationComplete = true;
                newNode = nullptr;
                nodesToMove.clear();
                originalPositions.clear();
                
                // Update final positions
                updateNodePositions();
            }
        }
    }

    void insertAtTail(int value, bool& animationComplete) {
        if (!isAnimating) {
            // Start insertion animation
            newNode = new Node(value);
            newNode->next = nullptr; // Tail's next is always nullptr
    
            // Store original positions (though they won't move in tail insertion)
            originalPositions.clear();
            nodesToMove.clear();
    
            // Set initial position for new node (start from the right side)
            if (tail != nullptr) {
                newNode->position = sf::Vector2f(
                    tail->position.x + 150.f, // Start 150px to the right of tail
                    tail->position.y
                );
            } else { // Empty list
                newNode->position = sf::Vector2f(
                    basePosition.x + 150.f, // Start 150px to the right of base
                    basePosition.y
                );
            }
    
            isAnimating = true;
            animationProgress = 0.f;
            animationComplete = false;
        } else {
            // Animate insertion
            animationProgress += animationSpeed * 0.5;
    
            // Calculate target position (where tail should be)
            sf::Vector2f targetPos;
            if (tail != nullptr) {
                targetPos = sf::Vector2f(
                    tail->position.x + nodeWidth + arrowLength,
                    tail->position.y
                );
            } else { // Empty list
                targetPos = basePosition;
            }
    
            // Smoothly move newNode to target position
            newNode->position = newNode->position + (targetPos - newNode->position) * animationProgress;
    
            // Check if animation is complete
            if (animationProgress >= 1.0f) {
                // Link the new node to the list
                if (tail != nullptr) {
                    tail->next = newNode; // Link previous tail to new node
                } else {
                    head = newNode; // First node in list
                }
                tail = newNode; // Update tail
                size++;
    
                isAnimating = false;
                animationComplete = true;
                newNode = nullptr;
                nodesToMove.clear();
                originalPositions.clear();
    
                // Update final positions (though they shouldn't change for tail insertion)
                updateNodePositions();
            }
        }
    }

    void insertAtanyposition(int value, int position, bool& animationComplete) {
        if (position < 1 || position > size + 1) {  // Invalid position
            animationComplete = true;
            return;
        }
    
        if (position == 1) {
            insertAtHead(value, animationComplete);
            return;
        }
        if (position == size + 1) {
            insertAtTail(value, animationComplete);
            return;
        }
    
        if (!isAnimating) {
            // Start insertion animation
            newNode = new Node(value);
    
            // Find the node after which we'll insert
            Node* previous = head;
            for (int i = 1; i < position - 1; i++) {
                previous = previous->next;
            }
    
            // Store original positions of nodes that will move
            originalPositions.clear();
            nodesToMove.clear();
    
            Node* current = previous->next; // Nodes that need to move right
            while (current != nullptr) {
                originalPositions.push_back(current->position);
                nodesToMove.push_back(current);
                current = current->next;
            }
    
            // Set new node's initial position (above the insertion point)
            newNode->position = previous->next->position - sf::Vector2f(0, 100);
            newNode->next = previous->next;
    
            isAnimating = true;
            animationProgress = 0.f;
            animationComplete = false;
        } 
        else {
            // Animate insertion
            animationProgress += animationSpeed * 0.5f;
    
            // 1. Animate new node coming down
            if (nodesToMove.size() > 0) {
                float targetY = originalPositions[0].y;
                newNode->position.y = originalPositions[0].y - 100 * (1 - animationProgress);
            }
    
            // 2. Animate existing nodes moving right
            for (size_t i = 0; i < nodesToMove.size(); i++) {
                float targetX = originalPositions[i].x + nodeWidth + arrowLength;
                nodesToMove[i]->position.x = originalPositions[i].x + 
                                          (targetX - originalPositions[i].x) * animationProgress;
            }
    
            // Check if animation is complete
            if (animationProgress >= 1.0f) {
                // Complete the insertion
                Node* prevNode = head;
                for (int i = 1; i < position - 1; i++) {
                    prevNode = prevNode->next;
                }
                
                prevNode->next = newNode;
                size++;
                
                isAnimating = false;
                animationComplete = true;
                newNode = nullptr;
                nodesToMove.clear();
                originalPositions.clear();
                
                // Update final positions
                updateNodePositions();
            }
        }
    }

    void deletenode(int position, bool& animationComplete){
        // Validate position first (1-based indexing)
        if (position < 1 || position > size) {
            animationComplete = true;
            return;
        }

        if (head == nullptr) {
            animationComplete = true;
            return;
        }

        if(!isAnimating){
            // Start deletion animation
            Node* toDelete = head;
            Node* prev = nullptr;

            for (int i = 1; i <= position - 1; i++) {
                prev = toDelete;
                toDelete = toDelete->next;
            }

            // Store original positions of nodes that will move
            originalPositions.clear();
            nodesToMove.clear();

            // Nodes to the right of the deleted node need to move left
            Node* current = toDelete->next;

            while (current != nullptr) {
                originalPositions.push_back(current->position);
                nodesToMove.push_back(current);
                current = current->next;
            }
            
            // Store the node to be deleted
            newNode = toDelete; 
            originalDeletePosition = toDelete->position;

            isAnimating = true;
            animationProgress = 0.f;
            animationComplete = false;
        }
        else{
            // Animate insertion
            animationProgress += animationSpeed * 0.5f;

            // 1. Animate the deleted node falling down
            if (newNode != nullptr) {
                newNode->position.y = originalDeletePosition.y + 100.f * animationProgress;
            }

            // 2. Animate nodes to the right moving left
            for (size_t i = 0; i < nodesToMove.size(); i++) {
                float targetX = originalPositions[i].x - (nodeWidth + arrowLength);
                nodesToMove[i]->position.x = originalPositions[i].x + 
                                        (targetX - originalPositions[i].x) * animationProgress;
            }

            // Check if animation is complete
            if (animationProgress >= 1.0f) {
                // Actually remove the node from the list
                Node* toDelete = newNode;
                
                if (position == 1) {
                    head = head->next;
                    if (head == nullptr) {
                        tail = nullptr;
                    }
                } 
                else {
                    Node* prev = head;
                    for (int i = 1; i < position - 1; i++) {
                        prev = prev->next;
                    }
                    prev->next = toDelete->next;
                    if (toDelete == tail) {
                        tail = prev;
                    }
                }

                delete toDelete;
                size--;
                
                isAnimating = false;
                animationComplete = true;
                newNode = nullptr;
                nodesToMove.clear();
                originalPositions.clear();
                
                // Update final positions
                updateNodePositions();
            }
        }
    }

    void updatenode(int position, int value, bool& animationComplete) {
        if (position < 1 || position > size) {
            animationComplete = true;
            return;
        }
    
        if (!isAnimating) {
            // Start update animation
            Node* nodeToUpdate = head;
            for (int i = 1; i < position; i++) {
                nodeToUpdate = nodeToUpdate->next;
            }
    
            // Store original position and old value
            originalPositions.clear();
            originalPositions.push_back(nodeToUpdate->position);
            oldValue = nodeToUpdate->data;
            
            // Create a temporary node for the old value
            newNode = new Node(oldValue);
            newNode->position = nodeToUpdate->position;
            
            // Update the actual node with new value and position it above
            nodeToUpdate->data = value;
            nodeToUpdate->position = nodeToUpdate->position - sf::Vector2f(0, 100);
    
            isAnimating = true;
            animationProgress = 0.f;
            animationComplete = false;
            currentOperation = Operation::Update;
        } else {
            // Animate both nodes simultaneously
            animationProgress += animationSpeed;
            
            Node* nodeToUpdate = head;
            for (int i = 1; i < position; i++) {
                nodeToUpdate = nodeToUpdate->next;
            }
    
            // Old node slides down
            newNode->position.y = originalPositions[0].y + 100.f * animationProgress;
            
            // New node comes down from above
            nodeToUpdate->position.y = originalPositions[0].y - 100.f * (1.f - animationProgress);
    
            if (animationProgress >= 1.0f) {
                // Animation complete - reset position
                nodeToUpdate->position = originalPositions[0];
                
                delete newNode;
                newNode = nullptr;
                isAnimating = false;
                animationComplete = true;
                currentOperation = Operation::None;
            }
        }
    }

    void searchNode(int value, bool& animationComplete) {
        static bool animating = false;
        static Node* current = nullptr;
        static Node* foundNode = nullptr;
        static float searchProgress = 0.f;
        static float foundHoldTime = 0.f;
        static const float HOLD_DURATION = 2.0f;
        static int currentIndex = 1;
        
        if (!animating) {
            // Initialize search
            current = head;
            foundNode = nullptr;
            searchProgress = 0.f;
            foundHoldTime = 0.f;
            currentIndex = 1;
            animating = true;
            animationComplete = false;
            currentOperation = Operation::Search;
            
            // Clear any previous highlights
            highlightedIndices.clear();
            foundBounce.assign(foundBounce.size(), 0.0f);
        } else {
            // If we've found a node and are in the hold phase
            if (foundNode != nullptr && foundHoldTime < HOLD_DURATION) {
                foundHoldTime += 1.0f / 60.0f;
                
                // Highlight only the found node
                highlightedIndices.clear();
                highlightedIndices.push_back(currentIndex);
                
                // Bounce effect for found node
                if (foundBounce.size() <= currentIndex) {
                    foundBounce.resize(currentIndex + 1, 0.0f);
                }
                foundBounce[currentIndex] = 1.0f;
                
                if (foundHoldTime >= HOLD_DURATION) {
                    // Reset after hold duration
                    highlightedIndices.clear();
                    foundBounce.assign(foundBounce.size(), 0.0f);
                    animating = false;
                    animationComplete = true;
                    currentOperation = Operation::None;
                }
                return;
            }
            
            // Animate search traversal
            if (current != nullptr) {
                searchProgress += animationSpeed;
                
                // Highlight current node being checked
                highlightedIndices.clear();
                highlightedIndices.push_back(currentIndex);
                
                if (searchProgress >= 1.0f) {
                    // Check current node
                    if (current->data == value) {
                        foundNode = current;
                        foundHoldTime = 0.f;
                    } else {
                        // Move to next node
                        current = current->next;
                        currentIndex++;
                        searchProgress = 0.f;
                    }
                }
            } else {
                // Search complete (element not found)
                highlightedIndices.clear();
                animating = false;
                animationComplete = true;
                currentOperation = Operation::None;
            }
        }
    }

    void clear() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        size = 0;
    }


};

class StackVisualizer {
private:    
    std::vector<int> stack;
    sf::Vector2f position;
    float cellWidth;
    float cellHeight;
    sf::Font& font;
    sf::Color defaultColor;

    // Animation variables
    bool isAnimating = false;
    float animationProgress = 0.f;
    int animatingValue = 0;
    sf::Vector2f animatingPosition;

    // Container dimensions
    const float containerX = 1000.f;
    const float containerY = 200.f;
    const float containerWidth = 80.f;
    const float containerHeight = 450.f;
    const float stackBaseX = containerX + containerWidth - 10.5f; // Right edge minus cell width
    const float stackBaseY = containerY + containerHeight - 10.f; // Bottom minus padding

    Operation currentOperation = Operation::None;

public: 
    float animationSpeed = 0.03f;  // Increased animation speed

public:
    StackVisualizer(sf::Font& fontRef, sf::Vector2f pos, 
                    float width, float height)
        : font(fontRef), position(pos), cellWidth(width), cellHeight(height),
        defaultColor(sf::Color(70, 130, 180)) // Steel blue          
    {
        // Initialize with some sample data
        stack = {10, 20, 30, 40, 50};
    }

    void drawContainer(sf::RenderWindow& window) {
        // Draw three sides (left, bottom, right) - no top line
        sf::VertexArray walls(sf::Lines, 6); // 3 lines = 6 vertices
        
        // Left wall (vertical)
        walls[0].position = sf::Vector2f(containerX, containerY + containerHeight);
        walls[1].position = sf::Vector2f(containerX, containerY);
        
        // Bottom platform (horizontal)
        walls[2].position = sf::Vector2f(containerX, containerY + containerHeight);
        walls[3].position = sf::Vector2f(containerX + containerWidth, containerY + containerHeight);
        
        // Right wall (vertical)
        walls[4].position = sf::Vector2f(containerX + containerWidth, containerY);
        walls[5].position = sf::Vector2f(containerX + containerWidth, containerY + containerHeight);

        for (int i = 0; i < 6; ++i) walls[i].color = sf::Color(150, 150, 150);
        
        window.draw(walls);

        // Add base
        sf::RectangleShape base(sf::Vector2f(containerWidth + 20, 10));
        base.setPosition(containerX - 10, containerY + containerHeight);
        base.setFillColor(sf::Color(80, 80, 80));
        window.draw(base);
    }

    void draw(sf::RenderWindow& window) {
        // Draw container 
        drawContainer(window);
        
        // Draw stack elements
        for (size_t i = 0; i < stack.size(); ++i) {
            // Skip drawing the top element if we're animating a pop or peek
            if (isAnimating && i == stack.size() - 1 && 
                (currentOperation == Operation::Pop || currentOperation == Operation::Peek)) {
                continue;
            }
            
            // Calculate position (stack grows upward)
            float x = stackBaseX - cellWidth;
            float y = stackBaseY - (i + 1) * cellHeight;
            
            // Draw stack element
            sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
            cell.setPosition(x, y);
            cell.setFillColor(defaultColor);
            cell.setOutlineThickness(2.f);
            cell.setOutlineColor(sf::Color::Black);
            window.draw(cell);
            
            // Draw value
            sf::Text valueText(std::to_string(stack[i]), font, 20);
            valueText.setPosition(
                x + cellWidth/2 - valueText.getLocalBounds().width/2,
                y + cellHeight/2 - valueText.getLocalBounds().height/2
            );
            valueText.setFillColor(sf::Color::White);
            window.draw(valueText);
        }
    
        // Draw animating element if pushing
        if (isAnimating && animatingValue != -1 && currentOperation == Operation::Push) {
            sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
            cell.setPosition(animatingPosition);
            cell.setFillColor(sf::Color(255, 255, 0, 200)); // Yellow for animation
            cell.setOutlineThickness(2.f);
            cell.setOutlineColor(sf::Color::Black);
            window.draw(cell);
            
            sf::Text valueText(std::to_string(animatingValue), font, 20);
            valueText.setPosition(
                animatingPosition.x + cellWidth/2 - valueText.getLocalBounds().width/2,
                animatingPosition.y + cellHeight/2 - valueText.getLocalBounds().height/2
            );
            valueText.setFillColor(sf::Color::White);
            window.draw(valueText);
        }
    
        // Draw animating element if popping
        if (isAnimating && animatingValue != -1 && currentOperation == Operation::Pop) {
            sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
            cell.setPosition(animatingPosition);
            cell.setFillColor(sf::Color::Magenta); // Different color for pop
            cell.setOutlineThickness(2.f);
            cell.setOutlineColor(sf::Color::Black);
            window.draw(cell);
            
            sf::Text valueText(std::to_string(animatingValue), font, 20);
            valueText.setPosition(
                animatingPosition.x + cellWidth/2 - valueText.getLocalBounds().width/2,
                animatingPosition.y + cellHeight/2 - valueText.getLocalBounds().height/2
            );
            valueText.setFillColor(sf::Color::White);
            window.draw(valueText);
        }
    
        // Draw animating element if peeking
        if (isAnimating && animatingValue != -1 && currentOperation == Operation::Peek) {
            sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
            cell.setPosition(animatingPosition);
            cell.setFillColor(sf::Color::Cyan); // Different color for peek
            cell.setOutlineThickness(2.f);
            cell.setOutlineColor(sf::Color::Black);
            window.draw(cell);
            
            sf::Text valueText(std::to_string(animatingValue), font, 20);
            valueText.setPosition(
                animatingPosition.x + cellWidth/2 - valueText.getLocalBounds().width/2,
                animatingPosition.y + cellHeight/2 - valueText.getLocalBounds().height/2
            );
            valueText.setFillColor(sf::Color::White);
            window.draw(valueText);
        }
    }

    void pushvalue(int value, bool& animationComplete) {
        if (!isAnimating) {
            // Start push animation
            animatingValue = value;
            animationProgress = 0.f;
            isAnimating = true;
            animationComplete = false;
            currentOperation = Operation::Push;
            
            // Start position (just above the container)
            animatingPosition = sf::Vector2f(
                stackBaseX - cellWidth,  // Same x position as other elements
                containerY - cellHeight  // Just above the container
            );
        } 
        else {
            // Animate the push
            animationProgress += animationSpeed * 0.05;
            
            // Calculate target position (top of stack)
            float targetY = stackBaseY - (stack.size() + 1) * cellHeight;
            
            // Smooth animation using linear interpolation
            animatingPosition.y = animatingPosition.y + 
                                (targetY - animatingPosition.y) * animationProgress;
            
            // Check if animation is complete
            if (animationProgress >= 1.0f) {
                // Actually push the value onto the stack
                stack.push_back(animatingValue);
                
                isAnimating = false;
                animationComplete = true;
                animatingValue = -1;  // Reset the animating value
                currentOperation = Operation::None;
            }
        }
    }

    void popvalue(bool& animationComplete) {
        if (stack.empty()) {
            animationComplete = true;
            return;
        }
    
        if (!isAnimating) {
            // Start pop animation
            animatingValue = stack.back();
            animationProgress = 0.f;
            isAnimating = true;
            animationComplete = false;
            currentOperation = Operation::Pop;
            
            // Set initial position (top of stack)
            animatingPosition = sf::Vector2f(
                stackBaseX - cellWidth, 
                stackBaseY - stack.size() * cellHeight
            );
        } 
        else {
            // Animate the pop
            animationProgress += animationSpeed * 0.05;
            
            // Move the element upwards (off the screen)
            animatingPosition.y = animatingPosition.y - (5.f * animationProgress);
            
            // Check if animation is complete
            if (animationProgress >= 1.0f) {
                // Actually pop the value from the stack
                stack.pop_back();
                
                isAnimating = false;
                animationComplete = true;
                animatingValue = -1;
                currentOperation = Operation::None;
            }
        }
    }

    void peekvalue(bool& animationComplete) {
        static bool isPeekAnimating = false;
        static float peekProgress = 0.f;
        static float holdTime = 0.f;
        static sf::Vector2f originalPosition;
        const float PEEK_DURATION = 2.0f; // 2 seconds
        const float PEEK_HEIGHT = 50.f;   // How much to slide up
        
        if (!isPeekAnimating && !stack.empty()) {
            // Start peek animation
            isPeekAnimating = true;
            peekProgress = 0.f;
            holdTime = 0.f;
            animationComplete = false;
            currentOperation = Operation::Peek;
            
            // Store original position
            originalPosition = sf::Vector2f(
                stackBaseX - cellWidth,
                stackBaseY - stack.size() * cellHeight
            );
            
            // Set animating value to the top element
            animatingValue = stack.back();
            animatingPosition = originalPosition;
            
            isAnimating = true;
        } 
        else if (isPeekAnimating) {
            // Animate the peek
            if (peekProgress < 1.0f) {
                // Slide up animation
                peekProgress += animationSpeed * 2.0f; // Faster animation
                
                // Calculate new position (ease out)
                float easedProgress = 1.f - (1.f - peekProgress) * (1.f - peekProgress); // Ease out quad
                animatingPosition.y = originalPosition.y - PEEK_HEIGHT * easedProgress;
            } 
            else {
                // Hold at peeked position
                holdTime += 1.0f / 60.f; // Assuming 60fps
                
                if (holdTime >= PEEK_DURATION) {
                    // Return to original position
                    float returnProgress = (holdTime - PEEK_DURATION) * animationSpeed * 2.0f;
                    
                    if (returnProgress >= 1.0f) {
                        // Animation complete
                        isPeekAnimating = false;
                        animationComplete = true;
                        currentOperation = Operation::None;
                        animatingValue = -1;
                        isAnimating = false;
                    } 
                    else {
                        // Slide back down
                        animatingPosition.y = originalPosition.y - PEEK_HEIGHT + 
                                             PEEK_HEIGHT * returnProgress;
                    }
                }
            }
        } 
        else {
            // Stack is empty, just complete immediately
            animationComplete = true;
        }
    }
};

class QueueVisualizer{
private:    
    std::deque<int> queue;
    sf::Vector2f position;
    float cellWidth;
    float cellHeight;
    sf::Font& font;
    sf::Color defaultColor;

    // Animation variables
    bool isAnimating = false;
    float animationProgress = 0.f;
    int animatingValue = 0;
    sf::Vector2f animatingPosition;

    std::vector<sf::Vector2f> currentPositions; // Actual drawn positions
    std::vector<sf::Vector2f> cellPositions;    //  Target position

    // Container dimensions
    const float containerX = 200.f;
    const float containerY = 550.f;
    const float containerWidth = 600.f;
    const float containerHeight = 80.f;
    const float stackBaseX = containerX + 20.f; 
    const float stackBaseY = containerY + 10.f; 


    Operation currentOperation = Operation::None;

public: 
    float animationSpeed = 0.03f;  // Increased animation speed

public:
    QueueVisualizer(sf::Font& fontRef, sf::Vector2f pos, 
                    float width, float height)
        : font(fontRef), position(pos), cellWidth(width), cellHeight(height),
        defaultColor(sf::Color(70, 130, 180)) // Steel blue          
    {
        // Initialize with sample data
        queue = {10, 20, 30, 40};
        updateCellPositions();
        currentPositions = cellPositions;
    }    

    void updateCellPositions() {
        cellPositions.clear();
        for (size_t i = 0; i < queue.size(); ++i) {
            cellPositions.emplace_back(
                stackBaseX + i * cellWidth,
                stackBaseY
            );
        }
        currentPositions.resize(queue.size());
    }

    void drawContainer(sf::RenderWindow& window){
        // Draw only top and bottom lines (no left/right sides)
        sf::VertexArray lines(sf::Lines, 4); // 2 lines = 4 vertices

        // Top line (from left to right)
        lines[0].position = sf::Vector2f(containerX, containerY);
        lines[1].position = sf::Vector2f(containerX + containerWidth, containerY);

        // Bottom line (from left to right)
        lines[2].position = sf::Vector2f(containerX, containerY + containerHeight);
        lines[3].position = sf::Vector2f(containerX + containerWidth, containerY + containerHeight);

        // Set color for all vertices
        for (int i = 0; i < 4; ++i) {
            lines[i].color = sf::Color(150, 150, 150); // Gray color
        }

        window.draw(lines);

        // Add base
        sf::RectangleShape base(sf::Vector2f(containerWidth + 20, 10));
        base.setPosition(containerX - 10, containerY + containerHeight+5.f);
        base.setFillColor(sf::Color(80, 80, 80));
        window.draw(base);
    }

    void draw(sf::RenderWindow& window) {
        drawContainer(window);
    
        // Draw all queue elements except the animating one
        for (size_t i = 0; i < queue.size(); ++i) {
            // Skip drawing the first element during dequeue animation
            if (isAnimating && (currentOperation == Operation::Dequeue || currentOperation == Operation::Front) && i == 0) {
                continue;
            }
    
            float x = currentPositions[i].x;
            float y = currentPositions[i].y;
    
            sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
            cell.setPosition(x, y);
            cell.setFillColor(defaultColor);
            cell.setOutlineThickness(2.f);
            cell.setOutlineColor(sf::Color::Black);
            window.draw(cell);
    
            sf::Text valueText(std::to_string(queue[i]), font, 20);
            valueText.setPosition(
                x + cellWidth / 2 - valueText.getLocalBounds().width / 2,
                y + cellHeight / 2 - valueText.getLocalBounds().height / 2
            );
            valueText.setFillColor(sf::Color::White);
            window.draw(valueText);
        }
    
        // Draw animating element during enqueue
        if (isAnimating && animatingValue != -1 && currentOperation == Operation::Enqueue) {
            sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
            cell.setPosition(animatingPosition);
            cell.setFillColor(sf::Color(255, 255, 0, 80)); // Yellow, semi-transparent
            cell.setOutlineThickness(2.f);
            cell.setOutlineColor(sf::Color::Black);
            window.draw(cell);
    
            sf::Text valueText(std::to_string(animatingValue), font, 20);
            valueText.setPosition(
                animatingPosition.x + cellWidth / 2 - valueText.getLocalBounds().width / 2,
                animatingPosition.y + cellHeight / 2 - valueText.getLocalBounds().height / 2
            );
            valueText.setFillColor(sf::Color::White);
            window.draw(valueText);
        }
    
        // Draw animating element during dequeue
        if (isAnimating && currentOperation == Operation::Dequeue && animatingValue != -1) {
            sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
            cell.setPosition(animatingPosition);
            cell.setFillColor(sf::Color(255, 0, 255, 200)); // Magenta with fading alpha
    
            cell.setOutlineThickness(2.f);
            cell.setOutlineColor(sf::Color::Black);
            window.draw(cell);
    
            sf::Text valueText(std::to_string(animatingValue), font, 20);
            valueText.setPosition(
                animatingPosition.x + cellWidth / 2 - valueText.getLocalBounds().width / 2,
                animatingPosition.y + cellHeight / 2 - valueText.getLocalBounds().height / 2
            );
            valueText.setFillColor(sf::Color(255, 255, 255));
            window.draw(valueText);
        }

        // Draw animating element during front operation
        if (isAnimating && currentOperation == Operation::Front && animatingValue != -1) {
            sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
            cell.setPosition(animatingPosition);
            cell.setFillColor(sf::Color(0, 255, 255, 200));  
            cell.setOutlineThickness(2.f);
            cell.setOutlineColor(sf::Color::Black);
            window.draw(cell);

            sf::Text valueText(std::to_string(animatingValue), font, 20);
            valueText.setPosition(
                animatingPosition.x + cellWidth / 2 - valueText.getLocalBounds().width / 2,
                animatingPosition.y + cellHeight / 2 - valueText.getLocalBounds().height / 2
            );
            valueText.setFillColor(sf::Color(255, 255, 255));
            window.draw(valueText);
        }
    }
    

    void enqueuevalue(int value, bool& animationComplete){
        if(!isAnimating){
            // Start push animation
            animatingValue = value;
            animationProgress = 0.f;
            isAnimating = true;
            animationComplete = false;
            currentOperation = Operation::Enqueue;
    
            // Start position (just to the right of the current end)
            animatingPosition = sf::Vector2f(
                stackBaseX + queue.size() * cellWidth + 100.f,  
                stackBaseY  
            );
        }
        else {
            // Animate the push
            animationProgress += animationSpeed * 0.05;
            
            // Calculate target position (current end of the queue)
            float targetX = stackBaseX + queue.size() * cellWidth;
            
            // Smooth animation using linear interpolation
            animatingPosition.x = animatingPosition.x + 
                                (targetX - animatingPosition.x) * animationProgress;
            
            // Check if animation is complete
            if (animationProgress >= 1.0f) {
                // Actually push the value onto the queue
                queue.push_back(animatingValue);
                updateCellPositions();  // Update cell positions to include new element
                currentPositions = cellPositions;  // Sync current positions
                
                isAnimating = false;
                animationComplete = true;
                animatingValue = -1;  // Reset the animating value
                currentOperation = Operation::None;
            }
        }
    }

    void dequeuevalue(bool& animationComplete) {
        if (queue.empty()) return;
    
        static std::vector<sf::Vector2f> originalPositions;
        static float progress = 0.f;
    
        const float dequeuedSlideDistance = 100.f;
        const float elementShiftDistance = 60.f;
    
        if (!isAnimating) {
            originalPositions = currentPositions;
            progress = 0.f;
            isAnimating = true;
            currentOperation = Operation::Dequeue;
            animatingValue = queue.front();
            animatingPosition = originalPositions[0];
        } else {
            progress += animationSpeed * 0.05f;
            progress = std::min(progress, 1.0f);
    
            // Animate the dequeued element sliding left
            animatingPosition.x = originalPositions[0].x - (dequeuedSlideDistance * progress);
    
            // Animate remaining elements
            for (size_t i = 1; i < originalPositions.size(); ++i) {
                if (i < currentPositions.size()) {
                    float startX = originalPositions[i].x;
                    float targetX = originalPositions[i - 1].x;
                    currentPositions[i].x = startX + (targetX - startX) * progress;
                }
            }
    
            if (progress >= 1.0f) {
                queue.pop_front();
                updateCellPositions(); // updates `cellPositions`
                currentPositions = cellPositions;
    
                isAnimating = false;
                animationComplete = true;
                animatingValue = -1;
                currentOperation = Operation::None;
            }
        }
    }
    
    
    void frontvalue(bool& animationComplete) {
        static bool isFrontAnimating = false;
        static float frontProgress = 0.f;
        static float holdTime = 0.f;
        static sf::Vector2f originalPosition;
        const float FRONT_DURATION = 2.0f; // 2 seconds
        const float FRONT_DISTANCE = 50.f; // How far to slide left (smaller distance)
        
        if (!isFrontAnimating && !queue.empty()) {
            // Start front animation
            isFrontAnimating = true;
            frontProgress = 0.f;
            holdTime = 0.f;
            animationComplete = false;
            currentOperation = Operation::Front;
            
            // Store original position (front of queue)
            originalPosition = sf::Vector2f(
                stackBaseX,
                stackBaseY
            );
            
            // Set animating value to the front element
            animatingValue = queue.front();
            animatingPosition = originalPosition;
            
            isAnimating = true;
        } 
        else if (isFrontAnimating) {
            // Animate the front highlight
            if (frontProgress < 1.0f) {
                // Slide left animation
                frontProgress += animationSpeed * 2.0f; // Faster animation
                
                // Calculate new position (ease out)
                float easedProgress = 1.f - (1.f - frontProgress) * (1.f - frontProgress); // Ease out quad
                animatingPosition.x = originalPosition.x - FRONT_DISTANCE * easedProgress;
            } 
            else {
                // Hold at front position
                holdTime += 1.0f / 60.f; // Assuming 60fps
                
                if (holdTime >= FRONT_DURATION) {
                    // Return to original position
                    float returnProgress = (holdTime - FRONT_DURATION) * animationSpeed * 2.0f;
                    
                    if (returnProgress >= 1.0f) {
                        // Animation complete
                        isFrontAnimating = false;
                        animationComplete = true;
                        currentOperation = Operation::None;
                        animatingValue = -1;
                        isAnimating = false;
                    } 
                    else {
                        // Slide back right
                        animatingPosition.x = originalPosition.x - FRONT_DISTANCE + 
                                           FRONT_DISTANCE * returnProgress;
                    }
                }
            }
        } 
        else {
            // Queue is empty, just complete immediately
            animationComplete = true;
        }
    }
};

class BinaryTreeVisualizer {
public:
    class Node {
    public:
        int data;
        Node* left;
        Node* right;
        sf::Vector2f position;  

        Node(int d) : data(d), left(nullptr), right(nullptr), position(0, 0) {}
    };

    Node* root;
    int size;

    // Visualization properties
    float nodeRadius;
    float horizontalSpacing;  
    float verticalSpacing;    
    sf::Vector2f basePosition;
    sf::Font& font;

    // Color
    sf::Color defaultColor;
    sf::Color traversalColor;
    sf::Color highlightColor;

    // Animation state
    bool isAnimating;
    Node* animatingNode;
    sf::Vector2f animationStartPos;
    sf::Vector2f animationTargetPos;
    float animationProgress;

    Node* currentlyHighlightedNode = nullptr;  // Only track current node being visited
    float nodeHighlightTime = 0.0f;
    const float NODE_HIGHLIGHT_DURATION = 0.3f; // Short highlight duration
    bool isTraversing = false;
    size_t traversalStep = 0;
    std::vector<Node*> traversalPath;  // Still need to store full path for animation timing

    // Animation remove state variables
    Node* nodeToRemove = nullptr;
    Node* parent = nullptr;
    Node* successorToRemove = nullptr;
    Node* successorParentToRemove = nullptr;
    bool isLeftChild = false;
    bool isRemoving = false;
    float fadeProgress = 0.f;
    float moveProgress = 0.f;
    int animationPhase = 0;
    std::vector<Node*> nodesToMove;
    std::vector<sf::Vector2f> originalPositions;
    std::vector<sf::Vector2f> targetPositions;

    // For search operation 
    std::vector<Node*> searchPath; 
    float highlightProgress = 0.f;
    // sf::Color traversalColor = sf::Color(100, 200, 255);  // Light blue for traversal
    sf::Color foundColor = sf::Color(0, 255, 0);         // Green for found node
    bool isHighlightedFound = false;
    bool isSearching;
    Node* foundNode;

    bool isBlinking = false;
    float blinkTime = 0.0f;
    const float BLINK_DURATION = 4.0f; // Total blink duration
    const float BLINK_INTERVAL = 0.2f; // Time between color switches
    bool showFoundColor = false;

    Operation currentOperation;

public:
    float animationSpeed = 0.03f;

public:
    BinaryTreeVisualizer(sf::Font& fontRef, sf::Vector2f pos, float radius)
    : root(nullptr), size(0), nodeRadius(radius), 
    horizontalSpacing(300.f), verticalSpacing(100.f),
    basePosition(pos), font(fontRef),
    defaultColor(sf::Color(70, 130, 180)),
    traversalColor(sf::Color(255, 127, 80)),
    highlightColor(sf::Color(255, 165, 0)),
    isAnimating(false), animatingNode(nullptr), animationProgress(0.f),
    isRemoving(false), nodeToRemove(nullptr), parent(nullptr),
    isLeftChild(false), fadeProgress(0.f), moveProgress(0.f),
    animationPhase(0), currentOperation(Operation::None)
    {
    // Initialize with sample data (proper binary search tree structure)
    root = new Node(60);
    root->position = basePosition;

    // Left subtree
    root->left = new Node(55);
    root->left->position = sf::Vector2f(basePosition.x - horizontalSpacing, basePosition.y + verticalSpacing);

    // Right subtree
    root->right = new Node(65);
    root->right->position = sf::Vector2f(basePosition.x + horizontalSpacing, basePosition.y + verticalSpacing);

    // Left-left subtree (values less than 55)
    root->left->left = new Node(45);
    root->left->left->position = sf::Vector2f(basePosition.x - 1.5f * horizontalSpacing, basePosition.y + 2 * verticalSpacing);

    // Left-right subtree (values greater than 55 but less than 60)
    root->left->right = new Node(57);
    root->left->right->position = sf::Vector2f(basePosition.x - 0.5f * horizontalSpacing, basePosition.y + 2 * verticalSpacing);

    // Right-left subtree (values less than 65 but greater than 60)
    root->right->left = new Node(62);
    root->right->left->position = sf::Vector2f(basePosition.x + 0.5f * horizontalSpacing, basePosition.y + 2 * verticalSpacing);

    // Right-right subtree (values greater than 65)
    root->right->right = new Node(70);
    root->right->right->position = sf::Vector2f(basePosition.x + 1.5f * horizontalSpacing, basePosition.y + 2 * verticalSpacing);

    size = 3;
    }

    ~BinaryTreeVisualizer() {
        clearTree(root);
    }

    void clearTree(Node* node) {
        if (node) {
            clearTree(node->left);
            clearTree(node->right);
            delete node;
        }
    }

    void draw(sf::RenderWindow& window) {
        if (root) {
            drawNode(window, root);
        }
        if (isAnimating && animatingNode && !isTraversing) {
            // Special drawing for animating node
            sf::CircleShape node(nodeRadius);  // Changed radius to nodeRadius
            node.setPosition(animatingNode->position - sf::Vector2f(nodeRadius, nodeRadius));
            node.setFillColor(highlightColor);  // Changed animationColor to highlightColor
            node.setOutlineThickness(2.f);
            node.setOutlineColor(sf::Color::Black);
            window.draw(node);
            
            // Draw value text
            sf::Text text(std::to_string(animatingNode->data), font, 20);  // Changed value to data
            text.setPosition(
                animatingNode->position.x - text.getLocalBounds().width / 2,
                animatingNode->position.y - text.getLocalBounds().height / 2
            );
            text.setFillColor(sf::Color::White);
            window.draw(text);
        }
    }

    void drawNode(sf::RenderWindow& window, Node* node) {
        if (!node) return;
    
        // Always draw connections in white
        auto drawConnection = [&](Node* target) {
            if (!target) return;
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = node->position;
            line[1].position = target->position;
            line[0].color = line[1].color = sf::Color::White; // Connections always white
            window.draw(line);
        };
    
        // Draw connections to children
        drawConnection(node->left);
        drawConnection(node->right);
    
        // Draw the node
        sf::CircleShape nodeShape(nodeRadius);
        nodeShape.setPosition(node->position.x - nodeRadius, node->position.y - nodeRadius);
    
        // Node coloring logic
        if (currentOperation == Operation::Search) {
            if (node == foundNode) {
                // Blinking effect for found node
                nodeShape.setFillColor(showFoundColor ? foundColor : defaultColor);
            } 
            else if (node == currentlyHighlightedNode) {
                // Current node being searched
                nodeShape.setFillColor(traversalColor);
            }
            else {
                nodeShape.setFillColor(defaultColor);
            }
        }else if ((currentOperation == Operation::Inorder || currentOperation == Operation::Preorder || currentOperation == Operation::Postorder)&& node == currentlyHighlightedNode) {
            nodeShape.setFillColor(traversalColor); // Highlight current traversal node
        } else {
            nodeShape.setFillColor(defaultColor); // Default color
        }
    
        // Common node styling
        nodeShape.setOutlineThickness(2.f);
        nodeShape.setOutlineColor(sf::Color::Black);
        window.draw(nodeShape);
    
        // Draw node value
        sf::Text valueText(std::to_string(node->data), font, 20);
        valueText.setPosition(
            node->position.x - valueText.getLocalBounds().width/2,
            node->position.y - valueText.getLocalBounds().height/2
        );
        valueText.setFillColor(sf::Color::White);
        window.draw(valueText);
    
        // Recursively draw children
        drawNode(window, node->left);
        drawNode(window, node->right);
    }

    void insertnode(int value, bool& animationComplete) {
        if (!isAnimating) {
            // Reset traversal tracking
            traversalPath.clear();
            traversalStep = 0;
            currentlyHighlightedNode = nullptr;
            isTraversing = true;
    
            if (!root) {
                root = new Node(value);
                root->position = basePosition;
                size++;
                animationComplete = true;
                isTraversing = false;
                return;
            }
    
            Node* current = root;
            Node* parent = nullptr;
            bool isLeftChild = false;
            int level = 0;
    
            while (current && level < 4) {
                traversalPath.push_back(current);
                parent = current;
    
                if (value < current->data) {
                    current = current->left;
                    isLeftChild = true;
                } else {
                    current = current->right;
                    isLeftChild = false;
                }
                level++;
            }
    
            if (level >= 4) {
                animationComplete = true;
                isTraversing = false;
                return;
            }
    
            // Setup new node animation
            animatingNode = new Node(value);
    
            float levelSpacing = horizontalSpacing * std::pow(0.7f, level);
            animationTargetPos = isLeftChild ?
                sf::Vector2f(parent->position.x - levelSpacing, parent->position.y + verticalSpacing) :
                sf::Vector2f(parent->position.x + levelSpacing, parent->position.y + verticalSpacing);
    
            animationStartPos = sf::Vector2f(parent->position.x, parent->position.y);
            animatingNode->position = animationStartPos;
    
            animationProgress = 0.f;
            animationComplete = false;
            currentOperation = Operation::Insert;
    
            if (!traversalPath.empty()) {
                currentlyHighlightedNode = traversalPath[0];
                nodeHighlightTime = 0.f;
            }
            isAnimating = true;
        }
    
        // Always handle traversal update if isTraversing
        if (isTraversing && !traversalPath.empty()) {
            nodeHighlightTime += 1.0f / 60.0f; // Assuming 60 FPS
    
            if (nodeHighlightTime >= NODE_HIGHLIGHT_DURATION) {
                traversalStep++;
                nodeHighlightTime = 0.f;
    
                if (traversalStep < traversalPath.size()) {
                    currentlyHighlightedNode = traversalPath[traversalStep];
                } else {
                    currentlyHighlightedNode = nullptr;
                    isTraversing = false;
                }
            }
        }
    
        // Handle animation separately
        if (isAnimating && !isTraversing) {
            animationProgress += animationSpeed * 0.05;
            animatingNode->position = animationStartPos +
                (animationTargetPos - animationStartPos) * animationProgress;
    
            if (animationProgress >= 1.0f) {
                Node* current = root;
                Node* parent = nullptr;
                bool isLeftChild = false;
                int level = 0;
    
                while (current && level < 4) {
                    parent = current;
                    if (value < current->data) {
                        current = current->left;
                        isLeftChild = true;
                    } else {
                        current = current->right;
                        isLeftChild = false;
                    }
                    level++;
                }
    
                if (isLeftChild) {
                    parent->left = animatingNode;
                } else {
                    parent->right = animatingNode;
                }
    
                animatingNode->position = animationTargetPos;
                size++;
                isAnimating = false;
                animatingNode = nullptr;
                animationComplete = true;
                currentOperation = Operation::None;
                isTraversing = false;
                currentlyHighlightedNode = nullptr;
                traversalPath.clear();
            }
        }
    }
    

    void removeNode(int value, bool& animationComplete) {
        if (!isRemoving) {
            // Start removal process
            nodeToRemove = nullptr;
            parent = nullptr;
            successorToRemove = nullptr;
            successorParentToRemove = nullptr;
            
            // Find the node to remove and its parent
            Node* current = root;
            Node* prev = nullptr;
            bool found = false;
            bool isLeft = false;
            
            while (current && !found) {
                if (value < current->data) {
                    prev = current;
                    current = current->left;
                    isLeft = true;
                } 
                else if (value > current->data) {
                    prev = current;
                    current = current->right;
                    isLeft = false;
                } 
                else {
                    found = true;
                }
            }
            
            if (!found) {
                animationComplete = true;
                return;
            }
            
            nodeToRemove = current;
            parent = prev;
            isLeftChild = isLeft;
            
            // Store nodes that will move (only successor in this version)
            nodesToMove.clear();
            originalPositions.clear();
            targetPositions.clear();
    
            // Handle two-child case
            if (nodeToRemove->left && nodeToRemove->right) {
                // Find inorder successor
                Node* successor = nodeToRemove->right;
                Node* successorParent = nodeToRemove;
                
                while (successor->left) {
                    successorParent = successor;
                    successor = successor->left;
                }
                
                // Store movement info
                nodesToMove.push_back(successor);
                originalPositions.push_back(successor->position);
                targetPositions.push_back(nodeToRemove->position);
                
                // Store for later removal
                successorToRemove = successor;
                successorParentToRemove = successorParent;
            }
            else {
                // For 0 or 1 child, just mark children that need to move up
                if (nodeToRemove->left) {
                    nodesToMove.push_back(nodeToRemove->left);
                    originalPositions.push_back(nodeToRemove->left->position);
                    targetPositions.push_back(nodeToRemove->position);
                }
                else if (nodeToRemove->right) {
                    nodesToMove.push_back(nodeToRemove->right);
                    originalPositions.push_back(nodeToRemove->right->position);
                    targetPositions.push_back(nodeToRemove->position);
                }
            }
            
            isRemoving = true;
            animationComplete = false;
            fadeProgress = 0.f;
            moveProgress = 0.f;
            animationPhase = 0;
        } 
        else {
            if (animationPhase == 0) {
                // Phase 1: Fade out the node to remove
                fadeProgress += 0.02f; // Adjust speed as needed
                
                if (fadeProgress >= 1.0f) {
                    animationPhase = 1;
                    fadeProgress = 1.0f;
                }
            } 
            else {
                // Phase 2: Move successor/replacement to deleted node's position
                moveProgress += 0.02f; // Adjust speed as needed
                
                if (moveProgress >= 1.0f) {
                    // Complete the removal
                    if (nodeToRemove->left && nodeToRemove->right) {
                        // For two-child case: swap data and remove successor
                        nodeToRemove->data = successorToRemove->data;
                        performRemoval(successorToRemove, successorParentToRemove, 
                                     successorParentToRemove->left == successorToRemove);
                    }
                    else {
                        // For 0 or 1 child case: normal removal
                        performRemoval(nodeToRemove, parent, isLeftChild);
                    }
                    
                    // Update positions (only for moved nodes)
                    for (size_t i = 0; i < nodesToMove.size(); i++) {
                        nodesToMove[i]->position = targetPositions[i];
                    }
                    
                    animationComplete = true;
                    isRemoving = false;
                    return;
                }
                
                // Animate the movement
                for (size_t i = 0; i < nodesToMove.size(); i++) {
                    Node* node = nodesToMove[i];
                    node->position.x = originalPositions[i].x + 
                                     (targetPositions[i].x - originalPositions[i].x) * moveProgress;
                    node->position.y = originalPositions[i].y + 
                                     (targetPositions[i].y - originalPositions[i].y) * moveProgress;
                }
            }
        }
    }
    
    void performRemoval(Node* nodeToRemove, Node* parent, bool isLeftChild) {
        // Case 1: Node is a leaf
        if (!nodeToRemove->left && !nodeToRemove->right) {
            if (parent) {
                if (isLeftChild) {
                    parent->left = nullptr;
                } else {
                    parent->right = nullptr;
                }
            } else {
                root = nullptr;
            }
            delete nodeToRemove;
        }
        // Case 2: Node has one child
        else if (!nodeToRemove->left || !nodeToRemove->right) {
            Node* child = nodeToRemove->left ? nodeToRemove->left : nodeToRemove->right;
            
            if (parent) {
                if (isLeftChild) {
                    parent->left = child;
                } else {
                    parent->right = child;
                }
            } else {
                root = child;
            }
            delete nodeToRemove;
        }
    }    

    void searchNode(int value, bool& animationComplete) {
        const float TRAVERSAL_DELAY = 0.5f; // Time between node visits (in seconds)
        static float traversalTimer = 0.f;
        static float blinkTimer = 0.f;
        
        if (!isSearching) {
            // Initialize search
            isSearching = true;
            isBlinking = false;
            foundNode = nullptr;
            currentlyHighlightedNode = root;
            animationComplete = false;
            currentOperation = Operation::Search;
            traversalTimer = 0.f;
            blinkTimer = 0.f;
            return;
        }
    
        if (!foundNode) {
            // Traversal phase
            traversalTimer += animationSpeed * 0.05f;
    
            // Only move to next node after delay
            if (traversalTimer >= TRAVERSAL_DELAY) {
                traversalTimer = 0.f;
                
                if (currentlyHighlightedNode) {
                    if (value == currentlyHighlightedNode->data) {
                        // Found the node
                        foundNode = currentlyHighlightedNode;
                        isBlinking = true;
                        blinkTimer = 0.f;
                    }
                    else if (value < currentlyHighlightedNode->data) {
                        currentlyHighlightedNode = currentlyHighlightedNode->left;
                    }
                    else {
                        currentlyHighlightedNode = currentlyHighlightedNode->right;
                    }
                }
                else {
                    // Node not found
                    isSearching = false;
                    animationComplete = true;
                    currentOperation = Operation::None;
                    currentlyHighlightedNode = nullptr;
                }
            }
        }
        else {
            // Blinking phase for found node
            blinkTimer += animationSpeed * 0.05f;
            
            // Toggle color every BLINK_INTERVAL seconds
            if (static_cast<int>(blinkTimer / BLINK_INTERVAL) % 2 == 0) {
                showFoundColor = true;
            } else {
                showFoundColor = false;
            }
    
            // End blinking after BLINK_DURATION
            if (blinkTimer >= BLINK_DURATION) {
                isSearching = false;
                isBlinking = false;
                animationComplete = true;
                currentOperation = Operation::None;
                foundNode = nullptr;
                currentlyHighlightedNode = nullptr;
            }
        }
    }

    void inordertraversal(bool& animationComplete) {
        static std::vector<Node*> traversalPath;
        static size_t currentStep = 0;
        static float highlightTime = 0.0f;
        const float HIGHLIGHT_DURATION = 0.8f; // Time to highlight each node (in seconds)
    
        if (!isTraversing) {
            // Initialize traversal
            traversalPath.clear();
            currentStep = 0;
            highlightTime = 0.0f;
            
            // Perform in-order traversal and store the path
            std::function<void(Node*)> inorder = [&](Node* node) {
                if (!node) return;
                inorder(node->left);
                traversalPath.push_back(node);
                inorder(node->right);
            };
            
            inorder(root);
            
            if (traversalPath.empty()) {
                animationComplete = true;
                return;
            }
            
            isTraversing = true;
            animationComplete = false;
            currentOperation = Operation::Inorder;
            currentlyHighlightedNode = traversalPath[0];
        } else {
            // Update traversal animation
            highlightTime += 1.0f / 60.0f; // Assuming 60 FPS
            
            if (highlightTime >= HIGHLIGHT_DURATION) {
                // Move to next node
                currentStep++;
                highlightTime = 0.0f;
                
                if (currentStep < traversalPath.size()) {
                    currentlyHighlightedNode = traversalPath[currentStep];
                } else {
                    // Traversal complete
                    isTraversing = false;
                    animationComplete = true;
                    currentOperation = Operation::None;
                    currentlyHighlightedNode = nullptr;
                    traversalPath.clear();
                }
            }
        }
    }

    void preordertraversal(bool& animationComplete) {
        static std::vector<Node*> traversalPath;
        static size_t currentStep = 0;
        static float highlightTime = 0.0f;
        const float HIGHLIGHT_DURATION = 0.8f; // Time to highlight each node (in seconds)
    
        if (!isTraversing) {
            // Initialize traversal
            traversalPath.clear();
            currentStep = 0;
            highlightTime = 0.0f;
            
            // Perform pre-order traversal and store the path
            std::function<void(Node*)> preorder = [&](Node* node) {
                if (!node) return;
                traversalPath.push_back(node);  // Visit root first
                preorder(node->left);          // Then left subtree
                preorder(node->right);         // Then right subtree
            };
            
            preorder(root);
            
            if (traversalPath.empty()) {
                animationComplete = true;
                return;
            }
            
            isTraversing = true;
            animationComplete = false;
            currentOperation = Operation::Preorder;
            currentlyHighlightedNode = traversalPath[0];
        } else {
            // Update traversal animation
            highlightTime += 1.0f / 60.0f; // Assuming 60 FPS
            
            if (highlightTime >= HIGHLIGHT_DURATION) {
                // Move to next node
                currentStep++;
                highlightTime = 0.0f;
                
                if (currentStep < traversalPath.size()) {
                    currentlyHighlightedNode = traversalPath[currentStep];
                } else {
                    // Traversal complete
                    isTraversing = false;
                    animationComplete = true;
                    currentOperation = Operation::None;
                    currentlyHighlightedNode = nullptr;
                    traversalPath.clear();
                }
            }
        }
    }

    void postordertraversal(bool& animationComplete) {
        static std::vector<Node*> traversalPath;
        static size_t currentStep = 0;
        static float highlightTime = 0.0f;
        const float HIGHLIGHT_DURATION = 0.8f; // Time to highlight each node (in seconds)
    
        if (!isTraversing) {
            // Initialize traversal
            traversalPath.clear();
            currentStep = 0;
            highlightTime = 0.0f;
            
            // Perform post-order traversal and store the path
            std::function<void(Node*)> postorder = [&](Node* node) {
                if (!node) return;
                postorder(node->left);    // Visit left subtree first
                postorder(node->right);   // Then right subtree
                traversalPath.push_back(node); // Finally visit the node itself
            };
            
            postorder(root);
            
            if (traversalPath.empty()) {
                animationComplete = true;
                return;
            }
            
            isTraversing = true;
            animationComplete = false;
            currentOperation = Operation::Postorder;
            currentlyHighlightedNode = traversalPath[0];
        } else {
            // Update traversal animation
            highlightTime += 1.0f / 60.0f; // Assuming 60 FPS
            
            if (highlightTime >= HIGHLIGHT_DURATION) {
                // Move to next node
                currentStep++;
                highlightTime = 0.0f;
                
                if (currentStep < traversalPath.size()) {
                    currentlyHighlightedNode = traversalPath[currentStep];
                } else {
                    // Traversal complete
                    isTraversing = false;
                    animationComplete = true;
                    currentOperation = Operation::None;
                    currentlyHighlightedNode = nullptr;
                    traversalPath.clear();
                }
            }
        }
    }
};

void runarrayvisualizer(sf::Font& font){
    // Main application window
    sf::RenderWindow window(sf::VideoMode(1200, 672), "Array Visualizer");
    window.setFramerateLimit(60);

    // Create array visualizer
    ArrayVisualizer arrayVis(font, {50.f, 500.f}, 60.f, 60.f);
    arrayVis.animationSpeed = 0.05f;

    // Create input fields
    TextInput insertIndexInput(font, {50.f, 125.f}, {100.f, 40.f}, "Insert at:");
    TextInput insertValueInput(font, {175.f, 125.f}, {100.f, 40.f}, "Value:");
    TextInput removeIndexInput(font, {50.f, 225.f}, {100.f, 40.f}, "Remove at:");
    TextInput searchValueInput(font, {50.f, 325.f}, {100.f, 40.f}, "Search:");
    TextInput updateIndex(font, {475.f, 125.f}, {100.f, 40.f}, "Update at:");
    TextInput updatevalue(font, {600.f, 125.f}, {100.f, 40.f}, "Value:");
    
    // Create buttons
    std::vector<std::unique_ptr<Button>> buttons;
    buttons.emplace_back(new Button("Insert", font, {310.f, 125.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Remove", font, {310.f, 225.f}, {100.f, 40.f}, 
                        sf::Color(100, 0, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Search", font, {310.f, 325.f}, {100.f, 40.f}, 
                        sf::Color(100, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Update", font, {725.f, 125.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Back to Menu", font, {975.f, 25.f}, {200.f, 50.f}, 
                        sf::Color(150, 0, 0, 200), sf::Color::White));

    // Animation state tracking
    bool isAnimating = false;
    bool animationComplete = false;
    enum class Operation { None, Insert, Remove, Search, Update };
    Operation currentOperation = Operation::None;
    int operationIndex = 0;
    int operationValue = 0;

    // Update button actions
    buttons[0]->setAction([&]() {  // Insert button
        if (!isAnimating && !insertIndexInput.isEmpty() && !insertValueInput.isEmpty()) {
            operationIndex = std::stoi(insertIndexInput.getText());
            operationValue = std::stoi(insertValueInput.getText());
            if (operationIndex >= 0 && operationIndex <= arrayVis.getArray().size()) {
                currentOperation = Operation::Insert;
                isAnimating = true;
                animationComplete = false;
            }
        }
        insertIndexInput.clear();
        insertValueInput.clear();
    });

    buttons[1]->setAction([&]() {  // Remove button
        if (!isAnimating && !removeIndexInput.isEmpty()) {
            operationIndex = std::stoi(removeIndexInput.getText());
            if (operationIndex >= 0 && operationIndex < arrayVis.getArray().size()) {
                currentOperation = Operation::Remove;
                isAnimating = true;
                animationComplete = false;
            }
        }
        removeIndexInput.clear();
    });

    buttons[2]->setAction([&]() {  // Search button
        if (!isAnimating && !searchValueInput.isEmpty()) {
            operationValue = std::stoi(searchValueInput.getText());
            currentOperation = Operation::Search;
            isAnimating = true;
            animationComplete = false;
        }
        searchValueInput.clear();
    });

    buttons[3]->setAction([&]() {  // Update button
        if (!isAnimating && !updateIndex.isEmpty() && !updatevalue.isEmpty()) {
            operationIndex = std::stoi(updateIndex.getText());
            operationValue = std::stoi(updatevalue.getText());
            if (operationIndex >= 0 && operationIndex <= arrayVis.getArray().size()) {
                currentOperation = Operation::Update;
                isAnimating = true;
                animationComplete = false;
            }
        }
        updateIndex.clear();
        updatevalue.clear();
    });

    buttons[4]->setAction([&]() {  // Back to Menu button
        window.close();
    });

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle input fields
            if (!isAnimating) {
                insertIndexInput.handleEvent(event, window);
                insertValueInput.handleEvent(event, window);
                removeIndexInput.handleEvent(event, window);
                searchValueInput.handleEvent(event, window);
                updateIndex.handleEvent(event, window);
                updatevalue.handleEvent(event, window);
                
                for (auto& btn : buttons) {
                    btn->handleEvent(event, window);
                }
            }
        }

        // Handle animations
        if (isAnimating) {
            switch (currentOperation) {
                case Operation::Insert:
                    arrayVis.insert(operationIndex, operationValue, animationComplete);
                    break;
                case Operation::Remove:
                    arrayVis.remove(operationIndex, animationComplete);
                    break;
                case Operation::Search:
                    arrayVis.search(operationValue, animationComplete);
                    break;
                case Operation::Update:
                    arrayVis.updateValue(operationIndex, operationValue, animationComplete);
                    break;
                case Operation::None:
                    break;
            }
            
            if (animationComplete) {
                isAnimating = false;
                currentOperation = Operation::None;
                
                // Clear input fields if operation completed
                switch (currentOperation) {
                    case Operation::Insert:
                        insertIndexInput.clear();
                        insertValueInput.clear();
                        break;
                    case Operation::Remove:
                        removeIndexInput.clear();
                        break;
                    case Operation::Search:
                        searchValueInput.clear();
                        break;
                    case Operation::Update:
                        updateIndex.clear();
                        updatevalue.clear();
                        break;
                    case Operation::None:
                        break;
                }
            }
        }
        
        window.clear(sf::Color(30, 30, 30));
        
        // Draw array
        arrayVis.draw(window);
        
        // Draw input fields and buttons
        insertIndexInput.draw(window);
        insertValueInput.draw(window);
        removeIndexInput.draw(window);
        searchValueInput.draw(window);
        updateIndex.draw(window);
        updatevalue.draw(window);
        
        for (auto& btn : buttons) {
            if (isAnimating) {
                btn->setAlpha(150);
            } else {
                btn->setAlpha(255);
            }
            btn->draw(window);
        }
        
        window.display();
    }
}

void runstringvisualizer(sf::Font& font){
    sf::RenderWindow window(sf::VideoMode(1200, 672), "String Visualizer");
    window.setFramerateLimit(60);

    // Create string visualizer
    StringVisualizer stringVis(font, {730.f, 500}, 40.f, 40.f);
    stringVis.animationSpeed = 0.05f;

    // Input fields
    TextInput insertIndexInput(font, {50.f, 125.f}, {100.f, 40.f}, "Insert at");
    TextInput insertValueInput(font, {175.f, 125.f}, {100.f, 40.f}, "Value:");
    TextInput removeIndexInput(font, {50.f, 225.f},{100.f, 40.f}, "Remove at:");
    TextInput searchValueInput(font, {50.f, 325.f}, {100.f, 40.f}, "Search:");
    TextInput updateIndex(font, {475.f, 125.f}, {100.f, 40.f}, "Update at:");
    TextInput updateValue(font, {600.f, 125.f}, {100.f, 40.f}, "Value:");
    
    // Create buttons
    std::vector<std::unique_ptr<Button>> buttons;
    buttons.emplace_back(new Button("Insert", font, {310.f, 125.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Remove", font, {310.f, 225.f}, {100.f, 40.f}, 
                        sf::Color(100, 0, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Search", font, {310.f, 325.f}, {100.f, 40.f}, 
                        sf::Color(100, 100, 0, 200), sf::Color::White));                    
    buttons.emplace_back(new Button("Update", font, {725.f, 125.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Back to Menu", font, {975.f, 25.f}, {200.f, 50.f}, 
                        sf::Color(150, 0, 0, 200), sf::Color::White));

    // Animation state tracking
    bool isAnimating = false;
    bool animationComplete = false;
    enum class Operation { None, Insert, Remove, Search, Update };
    Operation currentOperation = Operation::None;
    int operationIndex = 0;
    std::string operationValue;
    char operationValue2;


    // Update button actions
    buttons[0]->setAction([&]() {  // Insert button
        if (!isAnimating && !insertIndexInput.isEmpty() && !insertValueInput.isEmpty()) {
            operationIndex = std::stoi(insertIndexInput.getText());
            operationValue = insertValueInput.getText();
            if (operationIndex >= 0 && operationIndex <= stringVis.getString().size()) {
                currentOperation = Operation::Insert;
                isAnimating = true;
                animationComplete = false;
            }
        }
        insertIndexInput.clear();
        insertValueInput.clear();
    });
    
    buttons[1]->setAction([&]() {  // Remove button
        if (!isAnimating && !removeIndexInput.isEmpty()) {
            operationIndex = std::stoi(removeIndexInput.getText());
            if (operationIndex >= 0 && operationIndex < stringVis.getString().size()) {
                currentOperation = Operation::Remove;
                isAnimating = true;
                animationComplete = false;
            }
        }
        removeIndexInput.clear();
    });

    buttons[2]->setAction([&]() {  // Search button
        if (!isAnimating && !searchValueInput.isEmpty()) {
            operationValue2 = searchValueInput.getText()[0];
            currentOperation = Operation::Search;
            isAnimating = true;
            animationComplete = false;
        }
        searchValueInput.clear();
    });

    buttons[3]->setAction([&]() {  // Update button
        if (!isAnimating && !updateIndex.isEmpty() && !updateValue.isEmpty()) {
            operationIndex = std::stoi(updateIndex.getText());
            operationValue2 = updateValue.getText()[0];
            if (operationIndex >= 0 && operationIndex <= stringVis.getString().size()) {
                currentOperation = Operation::Update;
                isAnimating = true;
                animationComplete = false;
            }
        }
        updateIndex.clear();
        updateValue.clear();
    });

    buttons[4]->setAction([&]() {  // Back to Menu button
        window.close();
    });
    
    while(window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle input fields
            if (!isAnimating) {
                insertIndexInput.handleEvent(event, window);
                insertValueInput.handleEvent(event, window);
                removeIndexInput.handleEvent(event,window);
                searchValueInput.handleEvent(event, window);
                updateIndex.handleEvent(event,window);
                updateValue.handleEvent(event,window);
                
                for (auto& btn : buttons) {
                    btn->handleEvent(event, window);
                }
            }

        } 

        // Handle animations
        if (isAnimating) {
            switch (currentOperation) {
                case Operation::Insert:
                    stringVis.insert(operationIndex, operationValue, animationComplete);
                    break;
                case Operation::Remove:
                    stringVis.remove(operationIndex, animationComplete);
                    break;
                case Operation::Search:
                    stringVis.search(operationValue2, animationComplete);
                    break;    
                case Operation::Update:
                    stringVis.updatevalue(operationIndex, operationValue2, animationComplete);
                    break;

                case Operation::None:
                    break;
            }
            
            if (animationComplete) {
                isAnimating = false;
                currentOperation = Operation::None;
                
                // Clear input fields if operation completed
                switch (currentOperation) {
                    case Operation::Insert:
                        insertIndexInput.clear();
                        insertValueInput.clear();
                        break;
                    case Operation::Remove:
                        removeIndexInput.clear();
                        break;
                    case Operation::Search:
                        searchValueInput.clear();
                        break;    
                    case Operation::Update:
                        updateIndex.clear();
                        updateValue.clear();
                        break;    
                    case Operation::None:
                        break;
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        stringVis.draw(window);

        // Draw input fields and buttons
        insertIndexInput.draw(window);
        insertValueInput.draw(window);
        removeIndexInput.draw(window);
        searchValueInput.draw(window);
        updateIndex.draw(window);
        updateValue.draw(window);

        for (auto& btn : buttons) {
            if (isAnimating) {
                btn->setAlpha(150);
            } else {
                btn->setAlpha(255);
            }
            btn->draw(window);
        }

        window.display();
    }
}

void runlinkedlistvisualizer(sf::Font& font){
    sf::RenderWindow window(sf::VideoMode(1200, 672), "Linked-List Visualizer");
    window.setFramerateLimit(60);

    Linkedlistvisualizer linkedlistVis(font, {50.f, 500.f}, 60.f, 60.f);
    linkedlistVis.animationSpeed = 0.05f;

    // Create input field 
    TextInput insertAtHeadValue(font, {50.f, 125.f}, {120.f, 40.f}, "Insertathead");
    TextInput insertAtTailValue(font, {50.f, 225.f}, {120.f, 40.f}, "Insertattail");
    TextInput insertIndexInput(font, {50.f, 325.f}, {120.f, 40.f}, "Insert at:");
    TextInput insertAtAnyPositionInput(font, {175.f, 325.f}, {120.f, 40.f}, "Value:");
    TextInput removeIndexInput(font, {475.f, 125.f}, {120.f, 40.f}, "Remove at:");
    TextInput updatePosition(font, {475.f, 225.f}, {100.f, 40.f}, "Update at:");
    TextInput updatevalue(font, {600.f, 225.f}, {100.f, 40.f}, "Value:");
    TextInput searchValueInput(font, {475.f, 325.f}, {120.f, 40.f}, "Search:");

    // Create buttons
    std::vector<std::unique_ptr<Button>> buttons;
    buttons.emplace_back(new Button("InsertAtHead", font, {310.f, 125.f}, {150.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("InsertAtTail", font, {310.f, 225.f}, {150.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Insert", font, {310.f, 325.f}, {150.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Remove", font, {725.f, 125.f}, {150.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Update", font, {725.f, 225.f}, {150.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Search", font, {725.f, 325.f}, {150.f, 40.f}, 
                        sf::Color(100, 100, 0, 200), sf::Color::White));                    
    buttons.emplace_back(new Button("Back to Menu", font, {975.f, 25.f}, {200.f, 50.f}, 
                        sf::Color(150, 0, 0, 200), sf::Color::White));
    
    // Animation state tracking
    bool isAnimating = false;
    bool animationComplete = false;
    enum class Operation { None, Insert, Remove, Search, Update, InsertAtHead, InsertAtTail, InsertAtAnyPosition};
    Operation currentOperation = Operation::None;
    int operationPosition;
    int operationValue;
    
    // Update button actions
    buttons[0]->setAction([&]() {  // Insert button
        if (!isAnimating && !insertAtHeadValue.isEmpty()) {
            operationValue = std::stoi(insertAtHeadValue.getText());
            currentOperation = Operation::InsertAtHead;
            isAnimating = true;
            animationComplete = false;
        }
        insertAtHeadValue.clear();
    });

    buttons[1]->setAction([&]() {  // Insert button
        if (!isAnimating && !insertAtTailValue.isEmpty()) {
            operationValue = std::stoi(insertAtTailValue.getText());
            currentOperation = Operation::InsertAtTail;
            isAnimating = true;
            animationComplete = false;
        }
        insertAtTailValue.clear();
    });

    buttons[2]->setAction([&]() {  // Insert button
        if (!isAnimating && !insertIndexInput.isEmpty() && !insertAtAnyPositionInput.isEmpty()) {
            operationPosition = std::stoi(insertIndexInput.getText());
            operationValue = std::stoi(insertAtAnyPositionInput.getText());
            if (operationPosition >= 1 && operationPosition <= linkedlistVis.size+1) {
                currentOperation = Operation::InsertAtAnyPosition;
                isAnimating = true;
                animationComplete = false;
            }
        }
        insertIndexInput.clear();
        insertAtAnyPositionInput.clear();
    });

    buttons[3]->setAction([&]() {  //Remove button
        if (!isAnimating && !removeIndexInput.isEmpty()) {
            operationPosition = std::stoi(removeIndexInput.getText());
            if (operationPosition >= 1 && operationPosition <= linkedlistVis.size) {
                currentOperation = Operation::Remove;
                isAnimating = true;
                animationComplete = false;
            }
        }
        removeIndexInput.clear();
    });

    buttons[4]->setAction([&]() {  // Update button
        if (!isAnimating && !updatePosition.isEmpty() && !updatevalue.isEmpty()) {
            operationPosition = std::stoi(updatePosition.getText());
            operationValue = std::stoi(updatevalue.getText());
            if (operationPosition >= 1 && operationPosition <= linkedlistVis.size) {
                currentOperation = Operation::Update;
                isAnimating = true;
                animationComplete = false;
            }
        }
        updatePosition.clear();
        updatevalue.clear();
    });

    buttons[5]->setAction([&]() {  // Search button 
        if (!isAnimating && !searchValueInput.isEmpty()) {
            operationValue = std::stoi(searchValueInput.getText());
            currentOperation = Operation::Search;
            isAnimating = true;
            animationComplete = false;
        }
        searchValueInput.clear();
    });
    
    buttons[6]->setAction([&]() {  // Back to Menu button
        window.close();
    });

    while(window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle Input fields
            if(!isAnimating){
                insertAtHeadValue.handleEvent(event, window);
                insertAtTailValue.handleEvent(event, window);
                insertIndexInput.handleEvent(event,window);
                insertAtAnyPositionInput.handleEvent(event,window);
                removeIndexInput.handleEvent(event, window);
                updatePosition.handleEvent(event, window);
                updatevalue.handleEvent(event, window);
                searchValueInput.handleEvent(event,window);
                
                for(auto& btn : buttons){
                    btn->handleEvent(event, window);
                }
            }
        }
        // Handle animations
        if(isAnimating){
            switch(currentOperation){
                case Operation::InsertAtHead:
                    linkedlistVis.insertAtHead(operationValue, animationComplete);
                    break;
                case Operation::InsertAtTail:
                    linkedlistVis.insertAtTail(operationValue, animationComplete);
                    break;
                case Operation::InsertAtAnyPosition:
                    linkedlistVis.insertAtanyposition(operationValue, operationPosition, animationComplete);
                    break;
                case Operation::Remove:
                    linkedlistVis.deletenode(operationPosition, animationComplete);
                    break;
                case Operation::Update:
                    linkedlistVis.updatenode(operationPosition, operationValue, animationComplete);
                    break; 
                case Operation::Search:
                    linkedlistVis.searchNode(operationValue, animationComplete);
                    break;    
                case Operation::None:
                    break;    
            }

            if(animationComplete){
                isAnimating = false;
                currentOperation = Operation::None;

                // Clear input fields if operation completed
                switch (currentOperation) {
                    case Operation::InsertAtHead:
                        insertAtHeadValue.clear();
                        break;
                    case Operation::InsertAtTail:
                        insertAtTailValue.clear();
                        break;
                    case Operation::InsertAtAnyPosition:
                        insertIndexInput.clear();
                        insertAtAnyPositionInput.clear();  
                        break; 
                    case Operation::Remove:
                        removeIndexInput.clear();
                        break;     
                    case Operation::Update:
                        updatePosition.clear();
                        updatevalue.clear();  
                    case Operation::Search:
                        searchValueInput.clear();
                        break;      
                    case Operation::None:
                        break;
                }            
            }
        }
        window.clear(sf::Color(30, 30, 30));
        
        // Draw array
        linkedlistVis.draw(window);
        
        // Draw input fields and buttons
        insertAtHeadValue.draw(window);
        insertAtTailValue.draw(window);
        insertIndexInput.draw(window);
        insertAtAnyPositionInput.draw(window);
        removeIndexInput.draw(window);
        updatePosition.draw(window);
        updatevalue.draw(window);
        searchValueInput.draw(window);

        for (auto& btn : buttons) {
            if (isAnimating) {
                btn->setAlpha(150);
            } else {
                btn->setAlpha(255);
            }
            btn->draw(window);
        }

        window.display();
    }
}

void runstackvisualizer(sf::Font& font){
    // Main application window
    sf::RenderWindow window(sf::VideoMode(1200, 672), "Stack Visualizer");
    window.setFramerateLimit(60);

    // Create stack visualizer
    StackVisualizer stackVis(font, {50.f, 500.f}, 60.f, 60.f);
    stackVis.animationSpeed = 0.3f;

    // Create input field
    TextInput pushValue(font, {50.f, 150.f}, {100.f, 40.f}, "Value:");

    // Create buttons
    std::vector<std::unique_ptr<Button>> buttons;
    buttons.emplace_back(new Button("Push", font, {310.f, 150.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Pop", font, {310.f, 250.f}, {100.f, 40.f}, 
                            sf::Color(100, 0, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Peek", font, {310.f, 350.f}, {100.f, 40.f}, 
                            sf::Color(100, 0, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Back to Menu", font, {50.f, 25.f}, {200.f, 50.f}, 
                        sf::Color(150, 0, 0, 200), sf::Color::White));
    
    // Animation state tracking
    bool isAnimating = false;
    bool animationComplete = false;
    enum class Operation { None, Push, Pop, Peek};
    Operation currentOperation = Operation::None;
    int operationValue = 0;


    // Update button actions
    buttons[0]->setAction([&]() {  // Insert button
        if (!isAnimating && !pushValue.isEmpty()) {
            operationValue = std::stoi(pushValue.getText());
            currentOperation = Operation::Push;
            isAnimating = true;
            animationComplete = false;
        }
        pushValue.clear();
    });

    buttons[1]->setAction([&]() {  // Pop button
        if (!isAnimating) {
            currentOperation = Operation::Pop;
            isAnimating = true;
            animationComplete = false;
        }
    });

    buttons[2]->setAction([&]() {  // Pop button
        if (!isAnimating) {
            currentOperation = Operation::Peek;
            isAnimating = true;
            animationComplete = false;
        }
    });

    buttons[3]->setAction([&]() {  // Back to Menu button
        window.close();
    });

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle input fields
            if (!isAnimating) {
                pushValue.handleEvent(event,window);
                
                for (auto& btn : buttons) {
                    btn->handleEvent(event, window);
                }
            }
        }

        // Handle animations
        if (isAnimating) {
            switch (currentOperation) {
                case Operation::Push:
                    stackVis.pushvalue(operationValue, animationComplete);
                    break;
                case Operation::Pop:
                    stackVis.popvalue(animationComplete);
                    break;    
                case Operation::Peek:
                    stackVis.peekvalue(animationComplete);
                    break;    
                case Operation::None:
                    break;
            }
            
            if (animationComplete) {
                isAnimating = false;
                currentOperation = Operation::None;
                
                // Clear input fields if operation completed
                switch (currentOperation) {
                    case Operation::Push:
                        pushValue.clear();
                        break;
                    case Operation::Pop:
                        break;    
                    case Operation::Peek:
                        break;    
                    case Operation::None:
                        break;
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        stackVis.draw(window);

        // Draw input fields and buttons
        pushValue.draw(window);
        
        for (auto& btn : buttons) {
            if (isAnimating) {
                btn->setAlpha(150);
            } else {
                btn->setAlpha(255);
            }
            btn->draw(window);
        }

        window.display();
    }        
}

void runqueuevisualizer(sf::Font& font){
    // Main application window
    sf::RenderWindow window(sf::VideoMode(1200, 672), "Queue Visualizer");
    window.setFramerateLimit(60);

    // Create queue visualizer
    QueueVisualizer queueVis(font, {50.f, 500.f}, 60.f, 60.f);
    queueVis.animationSpeed = 0.3f;

    // Create input field
    TextInput enqueueValue(font, {50.f, 150.f}, {100.f, 40.f}, "Value:");

    // Create buttons
    std::vector<std::unique_ptr<Button>> buttons;
    buttons.emplace_back(new Button("Push", font, {310.f, 150.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Pop", font, {310.f, 250.f}, {100.f, 40.f}, 
                        sf::Color(100, 0, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Peek", font, {310.f, 350.f}, {100.f, 40.f}, 
                        sf::Color(100, 0, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Back to Menu", font, {975.f, 25.f}, {200.f, 50.f}, 
                        sf::Color(150, 0, 0, 200), sf::Color::White));


    // Animation state tracking
    bool isAnimating = false;
    bool animationComplete = false;
    enum class Operation { None, Enqueue, Dequeue, Front};
    Operation currentOperation = Operation::None;
    int operationValue = 0;

    // Update button actions
    buttons[0]->setAction([&]() {  // Insert button
        if (!isAnimating && !enqueueValue.isEmpty()) {
            operationValue = std::stoi(enqueueValue.getText());
            currentOperation = Operation::Enqueue;
            isAnimating = true;
            animationComplete = false;
        }
        enqueueValue.clear();
    });

    buttons[1]->setAction([&]() {  // Insert button
        currentOperation = Operation::Dequeue;
        isAnimating = true;
        animationComplete = false;
    
    });

    buttons[2]->setAction([&]() {  // Peek button
        currentOperation = Operation::Front;
        isAnimating = true;
        animationComplete = false;
    });

    buttons[3]->setAction([&]() {  // Back to Menu button
        window.close();
    });

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle input fields
            if (!isAnimating) {
                enqueueValue.handleEvent(event,window);
                
                for (auto& btn : buttons) {
                    btn->handleEvent(event, window);
                }
            }
        }

        // Handle animations
        if (isAnimating) {
            switch (currentOperation) {
                case Operation::Enqueue:
                    queueVis.enqueuevalue(operationValue, animationComplete);
                    break;
                case Operation::Dequeue:
                    queueVis.dequeuevalue(animationComplete);
                    break;    
                case Operation::Front:
                    queueVis.frontvalue(animationComplete);
                    break;    
                case Operation::None:
                    break;
            }
            
            if (animationComplete) {
                isAnimating = false;
                currentOperation = Operation::None;
                
                // Clear input fields if operation completed
                switch (currentOperation) {
                    case Operation::Enqueue:
                    enqueueValue.clear();
                        break;
                    case Operation::Dequeue:
                        break;    
                    case Operation::Front:
                        break;    
                    case Operation::None:
                        break;
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        queueVis.draw(window);

        // Draw input fields and buttons
        enqueueValue.draw(window);
        
        for (auto& btn : buttons) {
            if (isAnimating) {
                btn->setAlpha(150);
            } else {
                btn->setAlpha(255);
            }
            btn->draw(window);
        }

        window.display();
    }        
}

void runbinarytreevisualizer(sf::Font& font){
    // Main application window
    sf::RenderWindow window(sf::VideoMode(1800, 800), "Binary tree Visualizer");
    window.setFramerateLimit(60);

    // Create queue visualizer
    BinaryTreeVisualizer binarytreeVis(font, {1100.f, 150.f}, 25.f);
    binarytreeVis.animationSpeed = 0.3f;

    // Create input field
    TextInput insertValue(font, {50.f, 150.f}, {100.f, 40.f}, "Value:");
    TextInput removeValue(font, {50.f, 250.f}, {100.f, 40.f}, "Value:");
    TextInput searchValue(font, {50.f, 350.f}, {100.f, 40.f}, "Value:");

    // Create buttons
    std::vector<std::unique_ptr<Button>> buttons;
    buttons.emplace_back(new Button("Insert", font, {210.f, 150.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Remove", font, {210.f, 250.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Search", font, {210.f, 350.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Inorder", font, {210.f, 450.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Preorder", font, {210.f, 550.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Postorder", font, {210.f, 650.f}, {100.f, 40.f}, 
                        sf::Color(0, 100, 0, 200), sf::Color::White));
    buttons.emplace_back(new Button("Back to Menu", font, {50.f, 25.f}, {200.f, 50.f}, 
                        sf::Color(150, 0, 0, 200), sf::Color::White));


    // Animation state tracking
    bool isAnimating = false;
    bool animationComplete = false;
    enum class Operation { None, Insert, Remove, Search, Inorder, Preorder, Postorder};
    Operation currentOperation = Operation::None;
    int operationValue = 0;

    // Update button actions
    buttons[0]->setAction([&]() {  // Insert button
        if (!isAnimating && !insertValue.isEmpty()) {
            operationValue = std::stoi(insertValue.getText());
            currentOperation = Operation::Insert;
            isAnimating = true;
            animationComplete = false;
        }
        insertValue.clear();
    });

    buttons[1]->setAction([&]() {  // Insert button
        if (!isAnimating && !removeValue.isEmpty()) {
            operationValue = std::stoi(removeValue.getText());
            currentOperation = Operation::Remove;
            isAnimating = true;
            animationComplete = false;
        }
        removeValue.clear();
    });

    buttons[2]->setAction([&]() {  // Insert button
        if (!isAnimating && !searchValue.isEmpty()) {
            operationValue = std::stoi(searchValue.getText());
            currentOperation = Operation::Search;
            isAnimating = true;
            animationComplete = false;
        }
        searchValue.clear();
    });

    buttons[3]->setAction([&]() {  // Inorder button
        if (!isAnimating) {
            currentOperation = Operation::Inorder;
            isAnimating = true;
            animationComplete = false;
        }
        
    });

    buttons[4]->setAction([&]() {  // Preorder button
        if (!isAnimating) {
            currentOperation = Operation::Preorder;
            isAnimating = true;
            animationComplete = false;
        }
        
    });

    buttons[5]->setAction([&]() {  // Postorder button
        if (!isAnimating) {
            currentOperation = Operation::Postorder;
            isAnimating = true;
            animationComplete = false;
        }
        
    });

    buttons[6]->setAction([&]() {  // Back to Menu button
        window.close();
    });

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle input fields
            if (!isAnimating) {
                insertValue.handleEvent(event,window);
                removeValue.handleEvent(event, window);
                searchValue.handleEvent(event, window);
                
                for (auto& btn : buttons) {
                    btn->handleEvent(event, window);
                }
            }
        }

        // Handle animations
        if (isAnimating) {
            switch (currentOperation) {
                case Operation::Insert:
                    binarytreeVis.insertnode(operationValue, animationComplete);
                    break;  
                case Operation::Remove:
                    binarytreeVis.removeNode(operationValue, animationComplete);
                    break;  
                case Operation::Search:
                    binarytreeVis.searchNode(operationValue, animationComplete);
                    break;  
                case Operation::Inorder:
                    binarytreeVis.inordertraversal(animationComplete);
                    break;  
                case Operation::Preorder:
                    binarytreeVis.preordertraversal(animationComplete);
                    break;  
                case Operation::Postorder:
                    binarytreeVis.postordertraversal(animationComplete);
                    break;  
                case Operation::None:
                    break;
            }
            
            if (animationComplete) {
                isAnimating = false;
                currentOperation = Operation::None;
                
                // Clear input fields if operation completed
                switch (currentOperation) {
                    case Operation::Insert:
                        insertValue.clear();
                        break;  
                    case Operation::Remove:
                        removeValue.clear();
                        break;  
                    case Operation::Search:
                        searchValue.clear();
                        break;  
                    case Operation::Inorder:
                        break;  
                    case Operation::Preorder:
                        break;  
                    case Operation::Postorder:
                        break;  
                    case Operation::None:
                        break;
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        binarytreeVis.draw(window);

        // Draw input fields and buttons
        insertValue.draw(window);
        removeValue.draw(window);
        searchValue.draw(window);
        
        for (auto& btn : buttons) {
            if (isAnimating) {
                btn->setAlpha(150);
            } else {
                btn->setAlpha(255);
            }
            btn->draw(window);
        }

        window.display();
    }    
}

int main() {
    // Load font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return EXIT_FAILURE;
    }

    // Main application loop
    while (true) {
        // Show start screen
        StartScreen startScreen;
        startScreen.run();
        if (!startScreen.isStartClicked()) {
            return 0;
        }

        // Data structure selection loop
        while (true) {
            DataStructureSelectionScreen selectionScreen;
            std::string selected = selectionScreen.run();
            
            // If selection window was closed, exit completely
            if (selected.empty()) {
                return 0;
            }
            
            if (selected == "back") {
                break; // Exit selection loop, go back to start screen
            }
            else if (selected == "array") {
                runarrayvisualizer(font);
            }
            else if (selected == "string") {
                runstringvisualizer(font);
            }
            else if (selected == "linked-list") {
                runlinkedlistvisualizer(font);
            }
            else if(selected == "stack"){
                runstackvisualizer(font);
            }
            else if(selected == "queue"){
                runqueuevisualizer(font);
            }
            else if(selected == "binary-tree"){
                runbinarytreevisualizer(font);
            }
        }
    }

    return 0;
}