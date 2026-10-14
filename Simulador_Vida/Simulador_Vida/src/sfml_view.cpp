#include "sfml_view.h"
#include <cmath>
#include <algorithm>
namespace
{
    const sf::Color BACKGROUND(13,20,30), PANEL(22,33,47), CONTROL(35,50,68), ACCENT(72,216,176);
    const float GRID_X = 30, GRID_Y = 165;
}
SfmlView::SfmlView()
    : laboratoryReference(nullptr), window(sf::VideoMode(1280,800), "Laboratorio de mundos vivos | SFML 2.5.1"), fontReady(false),
      playing(false), creating(false), patternMode(false), deleting(false), activeSpecies(1), focusedField(0),
      message("Pinta con clic izquierdo. Borra con clic derecho."), leftDown(false), rightDown(false)
{
    window.setFramerateLimit(60);
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int width = std::min(1280u, desktop.width > 40 ? desktop.width - 40 : desktop.width);
    unsigned int height = std::min(800u, desktop.height > 80 ? desktop.height - 80 : desktop.height);
    window.setSize(sf::Vector2u(width,height));
    window.setPosition(sf::Vector2i(10,20));
    window.setView(sf::View(sf::FloatRect(0,0,1280,800)));
    const char* paths[] = { "assets/DejaVuSans.ttf", "../assets/DejaVuSans.ttf", "../../assets/DejaVuSans.ttf",
        "../../../assets/DejaVuSans.ttf", "C:/Windows/Fonts/arial.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" };
    for (int i = 0; i < 6 && !fontReady; i++) fontReady = font.loadFromFile(paths[i]);
    fields[0] = "Mundo nuevo"; fields[1] = "20"; fields[2] = "25"; fields[3] = "3";
}
void SfmlView::text(const string& value, float x, float y, unsigned int size, sf::Color color)
{
    if (!fontReady) return;
    sf::Text label;
    label.setFont(font); label.setString(sf::String::fromUtf8(value.begin(), value.end()));
    label.setCharacterSize(size); label.setFillColor(color); label.setPosition(x,y); window.draw(label);
}
void SfmlView::rectangle(float x, float y, float width, float height, sf::Color color)
{
    sf::RectangleShape shape(sf::Vector2f(width,height)); shape.setPosition(x,y); shape.setFillColor(color); window.draw(shape);
}
bool SfmlView::hit(float x, float y, float width, float height) const
{ return pointer.x >= x && pointer.x < x+width && pointer.y >= y && pointer.y < y+height; }
bool SfmlView::button(const string& label, float x, float y, float width, bool selected, bool enabled)
{
    sf::Color color = selected ? sf::Color(39,104,93) : CONTROL;
    if (enabled && hit(x,y,width,32) && !creating && !deleting) color = sf::Color(53,78,98);
    rectangle(x,y,width,32,enabled ? color : sf::Color(27,36,46));
    text(label,x+9,y+6,14,enabled ? sf::Color(230,239,246) : sf::Color(105,119,134));
    return enabled && hit(x,y,width,32);
}
float SfmlView::cellSize(const World& world) const
{ return std::min(760.f / world.getColumns(), 550.f / world.getRows()); }
void SfmlView::drawWorld(const World& world)
{
    float size = cellSize(world);
    for (int r = 0; r < world.getRows(); r++) for (int c = 0; c < world.getColumns(); c++)
    {
        float x = GRID_X+c*size, y = GRID_Y+r*size;
        rectangle(x,y,size-1,size-1,sf::Color(27,40,54));
        int id = world.getCell(r,c);
        if (id == 0) continue;
        const SpeciesBase& species = world.getSpecies(id);
        unsigned int rgb = species.getColor();
        sf::Color color(static_cast<sf::Uint8>(rgb >> 16),static_cast<sf::Uint8>(rgb >> 8),static_cast<sf::Uint8>(rgb));
        int shape = species.getShape();
        if (shape == 0) rectangle(x+size*.16f,y+size*.16f,size*.65f,size*.65f,color);
        else
        {
            sf::CircleShape symbol(size*.38f, shape == 1 ? 24 : shape == 2 ? 3 : 4);
            symbol.setOrigin(size*.38f,size*.38f); symbol.setPosition(x+size*.48f,y+size*.48f);
            symbol.setFillColor(color); window.draw(symbol);
        }
        if (size >= 20) text(string(1,species.getSymbol()),x+size*.29f,y+size*.12f,
            static_cast<unsigned int>(size*.55f),sf::Color(12,24,35));
    }
    text("Izquierdo: pintar  |  Derecho: borrar  |  Arrastra para dibujar",30,722,13);
}

void SfmlView::draw(Laboratory& laboratory)
{
    window.clear(BACKGROUND);
    text("ENJAMBRE / EDICION GRAFICA",30,22,25,ACCENT);
    text("Etapa 06: pinta, borra y siembra. Gestion y tiempo llegan en la etapa 07.",30,54,13);
    Timeline* timeline=laboratory.getActive();
    if(timeline!=nullptr)
    {
        const World& world=timeline->getWorld();
        text(world.getName(),30,128,20);drawWorld(world);
        rectangle(810,130,440,585,PANEL);
        text("ESPECIE ACTIVA",830,142,16,ACCENT);
        for(int i=1;i<=world.getSpeciesCount();i++)
            button(string(1,world.getSpecies(i).getSymbol()),830.f+(i-1)*62,173,53,activeSpecies==i);
        text(world.getSpecies(activeSpecies).getName(),1090,179,14);
        text("Celdas vivas: "+to_string(world.getPopulation(0)),830,246,15);
        for(int i=1;i<=world.getSpeciesCount();i++)
            text(string(1,world.getSpecies(i).getSymbol())+": "+to_string(world.getPopulation(i)),830,280.f+i*32,15);
        text("W cuadrado | S circulo | G triangulo | Q rombo",830,668,12);
        button("Limpiar",400,751,100);button("Aleatorio",510,751,110);
        button(patternMode?"Patron: clic...":"Patron 3x3",630,751,150,patternMode);
    }
    text(message,830,733,12);
    if(!fontReady)window.setTitle("Falta assets/DejaVuSans.ttf");
    window.display();
}
void SfmlView::stop() { playing=false; leftDown=false; rightDown=false; playbackClock.restart(); }
void SfmlView::changed(Timeline& timeline, const string& detail)
{
    bool hadFuture = timeline.hasNext(); timeline.discardFuture(); stop();
    message = hadFuture ? "Futuro descartado. " + detail : detail;
}



void SfmlView::paint(Laboratory& laboratory, bool erase)
{
    Timeline* timeline=laboratory.getActive(); if (timeline==nullptr || creating || deleting) return;
    World& world=timeline->editWorld(); float size=cellSize(world);
    if (!hit(GRID_X,GRID_Y,world.getColumns()*size,world.getRows()*size)) return;
    int row=static_cast<int>((pointer.y-GRID_Y)/size), column=static_cast<int>((pointer.x-GRID_X)/size);
    if (patternMode && !erase)
    {
        if (world.seedPattern(row,column)) { patternMode=false; changed(*timeline,"Patron colocado."); }
        else { stop(); message="El patron 3x3 no cabe aqui."; }
    }
    else if (world.setCell(row,column,erase?0:activeSpecies))
    {
        bool left=leftDown, right=rightDown;
        changed(*timeline,erase?"Celda borrada.":"Celda pintada.");
        leftDown=left; rightDown=right; // Mantiene la edicion por arrastre.
    }
}
void SfmlView::click(Laboratory& laboratory)
{
    Timeline* timeline=laboratory.getActive();if(timeline==nullptr)return;
    World& world=timeline->editWorld();
    for(int i=1;i<=world.getSpeciesCount();i++)
        if(hit(830.f+(i-1)*62,173,53,32)){activeSpecies=i;return;}
    if(hit(400,751,100,32)){world.clear();changed(*timeline,"Mundo limpio.");return;}
    if(hit(510,751,110,32)){world.seedRandom();changed(*timeline,"Siembra aleatoria aplicada.");return;}
    if(hit(630,751,150,32)){stop();patternMode=!patternMode;message="Patron: .W. / ..S / GGG";return;}
    leftDown=true;paint(laboratory,false);
}
void SfmlView::processEvent(const sf::Event& event, Laboratory& laboratory)
{
    if (event.type==sf::Event::Closed) { window.close(); return; }
    if (event.type==sf::Event::Resized)
        window.setView(sf::View(sf::FloatRect(0,0,1280,800)));
    if (event.type==sf::Event::LostFocus) { stop(); return; }
    if (event.type==sf::Event::MouseButtonReleased)
    { if (event.mouseButton.button==sf::Mouse::Left) leftDown=false; if (event.mouseButton.button==sf::Mouse::Right) rightDown=false; }
    if (event.type==sf::Event::MouseButtonPressed)
    {
        pointer=window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x,event.mouseButton.y));
        if (event.mouseButton.button==sf::Mouse::Left) click(laboratory);
        else if (event.mouseButton.button==sf::Mouse::Right && !creating && !deleting) { rightDown=true; paint(laboratory,true); }
    }
    if (event.type==sf::Event::MouseMoved)
    {
        pointer=window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x,event.mouseMove.y));
        if (leftDown) paint(laboratory,false); else if (rightDown) paint(laboratory,true);
    }
    if (event.type!=sf::Event::KeyPressed) return;
    sf::Keyboard::Key key=event.key.code;
    Timeline* timeline=laboratory.getActive(); if (timeline==nullptr) return;
    if (key>=sf::Keyboard::Num1 && key<=sf::Keyboard::Num4)
    { int id=key-sf::Keyboard::Num1+1; if (id<=timeline->getWorld().getSpeciesCount()) activeSpecies=id; }
    else if (key==sf::Keyboard::Escape) { stop(); patternMode=false; }
}
void SfmlView::run(Laboratory& laboratory)
{
    laboratoryReference = &laboratory;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)) processEvent(event,laboratory);
        if (!window.isOpen()) break;
        pointer=window.mapPixelToCoords(sf::Mouse::getPosition(window));
        draw(*laboratoryReference);
    }
    laboratoryReference = nullptr;
}
