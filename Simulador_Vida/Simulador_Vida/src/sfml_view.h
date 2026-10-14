#pragma once
#include <SFML/Graphics.hpp>
#include "view.h"
class SfmlView : public View
{
private:

    Laboratory* laboratoryReference; 
    sf::RenderWindow window;
    sf::Font font;
    bool fontReady, playing, creating, patternMode, deleting;
    int activeSpecies, focusedField;
    string fields[4];
    string message;
    sf::Clock playbackClock;
    sf::Vector2f pointer;
    bool leftDown, rightDown;
    void text(const string& value, float x, float y, unsigned int size = 15, sf::Color color = sf::Color(222,232,241));
    void rectangle(float x, float y, float width, float height, sf::Color color);
    bool button(const string& label, float x, float y, float width, bool selected = false, bool enabled = true);
    bool hit(float x, float y, float width, float height) const;
    void draw(Laboratory& laboratory);
    void drawWorld(const World& world);
    void drawCreation();
    void click(Laboratory& laboratory);
    void paint(Laboratory& laboratory, bool erase);
    void processEvent(const sf::Event& event, Laboratory& laboratory);
    void changed(Timeline& timeline, const string& detail);
    void stop();
    void openCreation();
    void submitCreation(Laboratory& laboratory);
    static bool parseInteger(const string& value, int& result);
    float cellSize(const World& world) const;
public:
    SfmlView();
    void run(Laboratory& laboratory) override;
};
