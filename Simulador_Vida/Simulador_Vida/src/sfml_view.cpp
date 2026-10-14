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
void SfmlView::drawCreation()
{
    rectangle(0,0,1280,800,sf::Color(0,0,0,185));
    rectangle(390,180,500,425,PANEL);
    text("Crear un mundo",420,202,24,ACCENT);
    text("Todo se ingresa dentro de esta ventana.",420,240,14);
    const char* labels[4] = { "Nombre (max. 24 bytes)", "Filas (5-50)", "Columnas (5-50)", "Especies (3 o 4)" };
    for (int i = 0; i < 4; i++)
    {
        float y = 280.f+i*55;
        text(labels[i],420,y+7,14);
        rectangle(665,y,190,34,focusedField == i ? sf::Color(44,84,92) : CONTROL);
        text(fields[i] + (focusedField == i ? "|" : ""),674,y+7,13);
    }
    button("Crear",420,515,125); button("Cancelar",560,515,125);
    text("Tab: siguiente campo  |  Enter: crear  |  Esc: cancelar",420,566,12);
    text(message,420,590,11,sf::Color(255,190,130));
}
void SfmlView::draw(Laboratory& laboratory)
{
    window.clear(BACKGROUND);
    text("ENJAMBRE / LABORATORIO",30,22,25,ACCENT);
    text("Mundos independientes. Un cambio puede abrir otro futuro.",30,54,13);
    button("Nuevo",30,83,95); button("Duplicar",135,83,100,false,laboratory.getActive()!=nullptr);
    button("Eliminar",245,83,100,false,laboratory.getActive()!=nullptr);
    button("< Mundo",355,83,110,false,laboratory.getCount()>1);
    button("Mundo >",475,83,110,false,laboratory.getCount()>1);
    text("Mundos: "+to_string(laboratory.getCount()),605,91,14);
    rectangle(810,130,440,585,PANEL);
    Timeline* timeline = laboratory.getActive();
    if (timeline != nullptr)
    {
        const World& world = timeline->getWorld();
        if (activeSpecies > world.getSpeciesCount()) activeSpecies = 1;
        text("#"+to_string(laboratory.getActiveId())+"  "+world.getName(),30,128,20);
        text(to_string(world.getRows())+" x "+to_string(world.getColumns()),665,135,14);
        drawWorld(world);
        text("ESPECIE ACTIVA / REGLAS",830,142,16,ACCENT);
        for (int i = 1; i <= world.getSpeciesCount(); i++)
            button(string(1,world.getSpecies(i).getSymbol()),830.f+(i-1)*62,173,53,activeSpecies==i);
        text(world.getSpecies(activeSpecies).getName(),1090,179,14);
        text("Generacion: "+to_string(world.getGeneration())+"    Historial: "+to_string(timeline->getHistoryCount()),830,218,16);
        text("Celdas vivas: "+to_string(world.getPopulation(0)),830,246,15);
        string populations;
        for (int i = 1; i <= world.getSpeciesCount(); i++) populations += string(1,world.getSpecies(i).getSymbol())+": "+to_string(world.getPopulation(i))+"   ";
        text(populations,830,274,14);
        text("Nacimiento: total de vecinos vivos",830,310,14);
        text("Supervivencia: total de vecinos vivos",830,382,14);
        for (int n = 0; n <= 8; n++)
        {
            bool enabled = world.isMoore() || n <= 4;
            button(to_string(n),830.f+n*45,337,39,world.getSpecies(activeSpecies).getRule(true,n),enabled);
            button(to_string(n),830.f+n*45,409,39,world.getSpecies(activeSpecies).getRule(false,n),enabled);
        }
        text("Resaltado = regla habilitada. Clic para alternar.",830,450,12);
        button(world.isMoore()?"Moore (8)":"Von Neumann (4)",830,477,200);
        button(world.isWrapped()?"Borde: toroidal":"Borde: cerrado",1040,477,190);
        text("Velocidad: "+to_string(world.getSpeed())+" gen/s",830,526,14);
        button("-",1130,519,42); button("+",1184,519,42);
        button(world.isMutationEnabled()?"Mutacion: SI":"Mutacion: NO",830,566,170,world.isMutationEnabled());
        text(to_string(world.getMutationPercent())+" %",1010,573,15);
        button("-",1130,566,42); button("+",1184,566,42);
        text("Densidad: "+to_string(world.getDensity())+" %",830,621,14);
        button("-",1130,614,42); button("+",1184,614,42);
        text("W cuadrado | S circulo | G triangulo | Q rombo",830,668,12);
        button("< Atras",30,751,100,false,timeline->hasPrevious());
        button("Adelante >",140,751,120);
        button(playing?"Pausar":"Reproducir",270,751,120,playing);
        button("Limpiar",400,751,100);
        button("Aleatorio",510,751,110);
        button(patternMode?"Patron: clic...":"Patron 3x3",630,751,150,patternMode);
    }
    else text("No hay mundos. Usa Nuevo para crear uno.",30,170,20);
    text(message,830,733,12);
    text("Espacio: pausa | Flechas: tiempo | 1-4: especie | Tab: mundo",830,773,11);
    if (creating) drawCreation();
    if (deleting)
    {
        rectangle(0,0,1280,800,sf::Color(0,0,0,185)); rectangle(390,260,500,230,PANEL);
        text("Eliminar el mundo activo?",420,290,23);
        text("Se elimina tambien todo su historial.",420,335,15);
        button("Eliminar",420,400,140); button("Cancelar",580,400,140);
    }
    if (!fontReady) { window.setTitle("Falta fuente: copie assets/DejaVuSans.ttf junto al programa"); }
    window.display();
}
void SfmlView::stop() { playing=false; leftDown=false; rightDown=false; playbackClock.restart(); }
void SfmlView::changed(Timeline& timeline, const string& detail)
{
    bool hadFuture = timeline.hasNext(); timeline.discardFuture(); stop();
    message = hadFuture ? "Futuro descartado. " + detail : detail;
}
void SfmlView::openCreation()
{ stop(); creating=true; deleting=false; patternMode=false; focusedField=0; message=""; }
bool SfmlView::parseInteger(const string& value, int& result)
{
    if (value.empty() || value.size()>3) return false;
    result=0;
    for (char character : value) { if (character<'0' || character>'9') return false; result=result*10+(character-'0'); }
    return true;
}
void SfmlView::submitCreation(Laboratory& laboratory)
{
    int rows, columns, count;
    if (!parseInteger(fields[1],rows) || !parseInteger(fields[2],columns) || !parseInteger(fields[3],count)
        || !laboratory.create(fields[0],rows,columns,count))
    { message="Datos invalidos: nombre; tamano 5-50; especies 3-4."; return; }
    creating=false; activeSpecies=1; message="Mundo creado con B3,6 / S2,3.";
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
        leftDown=left; rightDown=right; 
    }
}
void SfmlView::click(Laboratory& laboratory)
{
    if (creating)
    {
        for (int i=0; i<4; i++) if (hit(665,280.f+i*55,190,34)) focusedField=i;
        if (hit(420,515,125,32)) submitCreation(laboratory);
        if (hit(560,515,125,32)) { creating=false; message="Creacion cancelada."; }
        return;
    }
    if (deleting)
    {
        if (hit(420,400,140,32)) { laboratory.deleteActive(); deleting=false; activeSpecies=1; message="Mundo eliminado."; }
        if (hit(580,400,140,32)) deleting=false;
        return;
    }
    if (hit(30,83,95,32)) { openCreation(); return; }
    Timeline* timeline=laboratory.getActive(); if (timeline==nullptr) return;
    if (hit(135,83,100,32)) { stop(); laboratory.duplicateActive(); patternMode=false; message="Copia independiente creada."; return; }
    if (hit(245,83,100,32)) { stop(); deleting=true; return; }
    if (hit(355,83,110,32) || hit(475,83,110,32))
    { stop(); patternMode=false; if (pointer.x<465) laboratory.selectPrevious(); else laboratory.selectNext(); return; }
    World& world=timeline->editWorld();
    for (int i=1; i<=world.getSpeciesCount(); i++) if (hit(830.f+(i-1)*62,173,53,32)) { activeSpecies=i; return; }
    for (int n=0; n<=8; n++)
    {
        if (!world.isMoore() && n>4) continue;
        bool birth=hit(830.f+n*45,337,39,32), survival=hit(830.f+n*45,409,39,32);
        if (birth || survival) { world.toggleRule(activeSpecies,birth,n); changed(*timeline,"Regla actualizada."); return; }
    }
    if (hit(830,477,200,32)) { world.toggleNeighborhood(); changed(*timeline,"Vecindario actualizado."); return; }
    if (hit(1040,477,190,32)) { world.toggleBorder(); changed(*timeline,"Borde actualizado."); return; }
    if (hit(830,566,170,32)) { world.toggleMutation(); changed(*timeline,"Mutacion actualizada."); return; }
    for (int i=0; i<2; i++)
    {
        int direction=i==0?-1:1;
        float x=i==0?1130.f:1184.f;
        if (hit(x,519,42,32)) { if (world.setSpeed(world.getSpeed()+direction)) changed(*timeline,"Velocidad actualizada."); return; }
        if (hit(x,566,42,32)) { if (world.setMutationPercent(world.getMutationPercent()+direction*5)) changed(*timeline,"Probabilidad actualizada."); return; }
        if (hit(x,614,42,32)) { if (world.setDensity(world.getDensity()+direction*5)) changed(*timeline,"Densidad actualizada."); return; }
    }
    if (hit(30,751,100,32)) { stop(); patternMode=false; timeline->backward(); message="Estado anterior restaurado."; return; }
    if (hit(140,751,120,32)) { stop(); patternMode=false; timeline->forward(); message="Generacion avanzada."; return; }
    if (hit(270,751,120,32)) { playing=!playing; patternMode=false; playbackClock.restart(); return; }
    if (hit(400,751,100,32)) { world.clear(); changed(*timeline,"Mundo limpio; conserva la generacion."); return; }
    if (hit(510,751,110,32)) { world.seedRandom(); changed(*timeline,"Siembra aleatoria aplicada."); return; }
    if (hit(630,751,150,32)) { stop(); patternMode=!patternMode; message="Patron: .W. / ..S / GGG"; return; }
    leftDown=true; paint(laboratory,false);
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
    if (creating && event.type==sf::Event::TextEntered)
    {
        sf::Uint32 character=event.text.unicode;
        string& value=fields[focusedField];
        if (character==8 && !value.empty())
        {
         
            while (!value.empty() && (static_cast<unsigned char>(value.back()) & 0xC0)==0x80) value.pop_back();
            if (!value.empty()) value.pop_back();
        }
        else if (character>=32 && character<127 && value.size()<(focusedField==0?24u:3u)
            && (focusedField==0 || (character>='0' && character<='9'))) value+=static_cast<char>(character);
        else if (focusedField==0 && character>=128 && character<=0x7FF && value.size()<=22)
        {
            value+=static_cast<char>(0xC0 | (character >> 6));
            value+=static_cast<char>(0x80 | (character & 0x3F));
        }
    }
    if (event.type!=sf::Event::KeyPressed) return;
    sf::Keyboard::Key key=event.key.code;
    if (creating)
    {
        if (key==sf::Keyboard::Escape) creating=false;
        else if (key==sf::Keyboard::Tab) focusedField=(focusedField+1)%4;
        else if (key==sf::Keyboard::Return) submitCreation(laboratory);
        return;
    }
    if (deleting) { if (key==sf::Keyboard::Escape) deleting=false; return; }
    if (key==sf::Keyboard::N) { openCreation(); return; }
    Timeline* timeline=laboratory.getActive(); if (timeline==nullptr) return;
    if (key==sf::Keyboard::Space) { playing=!playing; patternMode=false; playbackClock.restart(); }
    else if (key==sf::Keyboard::Left) { stop(); patternMode=false; timeline->backward(); }
    else if (key==sf::Keyboard::Right) { stop(); patternMode=false; timeline->forward(); }
    else if (key==sf::Keyboard::Tab) { stop(); patternMode=false; laboratory.selectNext(); }
    else if (key>=sf::Keyboard::Num1 && key<=sf::Keyboard::Num4)
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
        const Timeline* active=laboratory.getActive();
        if (playing && active!=nullptr && playbackClock.getElapsedTime().asSeconds()>=1.f/active->getWorld().getSpeed())
        { laboratory.getActive()->forward(); playbackClock.restart(); }
        pointer=window.mapPixelToCoords(sf::Mouse::getPosition(window));
        draw(*laboratoryReference);
    }
    laboratoryReference = nullptr;
}
