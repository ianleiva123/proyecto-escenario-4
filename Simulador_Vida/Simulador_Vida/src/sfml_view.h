#pragma once
#include "configuration.h"
#include "layout.h"
#include "view.h"
#include <SFML/Graphics.hpp>
class SfmlView : public View
{
  private:
    enum class Modal
    {
        None,
        Create,
        Delete,
        NewLab,
        Save,
        Load
    };
    sf::RenderWindow window;
    sf::Font font;
    Configuration configuration;
    bool fontReady;
    bool patternMode;
    bool leftDown;
    bool rightDown;
    bool resumeAfterModal;
    bool replaceInput;
    Modal modal;
    int focusedField;
    std::string fields[4];
    std::string fileName;
    std::string message;
    std::string notice;
    sf::Clock playbackClock;
    sf::Vector2f pointer;
    void text(const std::string &value, float x, float y, unsigned int size = 15,
              sf::Color color = sf::Color(222, 232, 241));
    void wrappedText(const std::string &value, float x, float y, int width = 115, unsigned int size = 13);
    void rectangle(const sf::FloatRect &bounds, sf::Color color);
    bool over(layout::Control control) const;
    void button(layout::Control control, const std::string &label, bool selected = false,
                bool enabled = true);
    float cellSize(const World &world) const;
    void drawWorld(const World &world);
    void drawTop(const Laboratory &laboratory);
    void drawInformation(const Laboratory &laboratory, const Timeline &timeline);
    void drawRules(const Laboratory &laboratory, const World &world);
    void drawSettings(const Laboratory &laboratory, const World &world);
    void drawTime(const Laboratory &laboratory, const Timeline &timeline);
    void drawModal();
    void draw(Laboratory &laboratory);
    void stop(Laboratory &laboratory);
    void changed(Laboratory &laboratory, const std::string &detail);
    void openModal(Modal value, Laboratory &laboratory);
    void cancelModal(Laboratory &laboratory);
    void confirmModal(Laboratory &laboratory);
    void submitCreation(Laboratory &laboratory);
    void processInput(sf::Uint32 character);
    void processKey(const sf::Event::KeyEvent &key, Laboratory &laboratory);
    void processEvent(const sf::Event &event, Laboratory &laboratory);
    bool handleTop(Laboratory &laboratory);
    bool handleRules(Laboratory &laboratory);
    bool handleSettings(Laboratory &laboratory);
    bool handleTime(Laboratory &laboratory);
    void click(Laboratory &laboratory);
    void paint(Laboratory &laboratory, bool erase);
    void stepBackward(Laboratory &laboratory);
    void stepForward(Laboratory &laboratory);
    static bool parseInteger(const std::string &value, int &result);
    bool validFileName() const;

  public:
    SfmlView(const Configuration &configuration, const std::string &startupMessage);
    void run(Laboratory &laboratory) override;
};
