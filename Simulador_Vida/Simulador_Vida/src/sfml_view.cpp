#include "sfml_view.h"
#include <algorithm>
#include <sstream>
namespace
{
const sf::Color BACKGROUND(13, 20, 30);
const sf::Color PANEL(22, 33, 47);
const sf::Color CONTROL(35, 50, 68);
const sf::Color ACCENT(72, 216, 176);
constexpr int FORM_FIELD_COUNT = 4;
constexpr int MAX_FILE_NAME = 48;
void removeCharacter(std::string &value)
{
    while (!value.empty() && (static_cast<unsigned char>(value.back()) & 0xC0) == 0x80)
    {
        value.pop_back();
    }
    if (!value.empty())
    {
        value.pop_back();
    }
}
} // namespace
using layout::Control;
SfmlView::SfmlView(const Configuration &config, const std::string &startupMessage)
    : window(
          sf::VideoMode(static_cast<unsigned int>(layout::WIDTH), static_cast<unsigned int>(layout::HEIGHT)),
          "Laboratorio Enjambre - SFML 2.5.1"),
      configuration(config), fontReady(false), patternMode(false), leftDown(false), rightDown(false),
      resumeAfterModal(false), replaceInput(false), modal(Modal::None), focusedField(0), fields{},
      fileName("laboratorio.txt"), message(startupMessage),
      notice("Pinta con clic izquierdo; borra con clic derecho."), playbackClock(), pointer()
{
    window.setFramerateLimit(layout::FRAME_LIMIT);
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int width = std::min(static_cast<unsigned int>(layout::WIDTH),
                                  desktop.width > 40 ? desktop.width - 40 : desktop.width);
    unsigned int height = std::min(static_cast<unsigned int>(layout::HEIGHT),
                                   desktop.height > 80 ? desktop.height - 80 : desktop.height);
    window.setSize(sf::Vector2u(width, height));
    window.setPosition(sf::Vector2i(10, 20));
    window.setView(sf::View(sf::FloatRect(0, 0, layout::WIDTH, layout::HEIGHT)));
    const char *paths[] = {"assets/DejaVuSans.ttf", "../../../assets/DejaVuSans.ttf",
                           "C:/Windows/Fonts/arial.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"};
    for (const char *path : paths)
    {
        if (font.loadFromFile(path))
        {
            fontReady = true;
            break;
        }
    }
    fields[0] = "Mundo nuevo";
    fields[1] = std::to_string(config.getDefaultSize().rows);
    fields[2] = std::to_string(config.getDefaultSize().columns);
    fields[3] = std::to_string(config.getDefaultSpecies());
}
void SfmlView::text(const std::string &value, float x, float y, unsigned int size, sf::Color color)
{
    if (!fontReady)
    {
        return;
    }
    sf::Text label;
    label.setFont(font);
    label.setString(sf::String::fromUtf8(value.begin(), value.end()));
    label.setCharacterSize(size);
    label.setFillColor(color);
    label.setPosition(x, y);
    window.draw(label);
}
void SfmlView::wrappedText(const std::string &value, float x, float y, int width, unsigned int size)
{
    std::stringstream words(value);
    std::string word;
    std::string line;
    while (words >> word)
    {
        if (!line.empty() && line.size() + word.size() > static_cast<std::size_t>(width))
        {
            text(line, x, y, size);
            y += size + 6.0f;
            line.clear();
        }
        if (!line.empty())
        {
            line += " ";
        }
        line += word;
    }
    text(line, x, y, size);
}
void SfmlView::rectangle(const sf::FloatRect &bounds, sf::Color color)
{
    sf::RectangleShape shape(sf::Vector2f(bounds.width, bounds.height));
    shape.setPosition(bounds.left, bounds.top);
    shape.setFillColor(color);
    window.draw(shape);
}
bool SfmlView::over(Control control) const
{
    return layout::bounds(control).contains(pointer);
}
void SfmlView::button(Control control, const std::string &label, bool selected, bool enabled)
{
    sf::FloatRect bounds = layout::bounds(control);
    sf::Color color = selected ? sf::Color(39, 104, 93) : CONTROL;
    if (enabled && over(control) && modal == Modal::None)
    {
        color = sf::Color(53, 78, 98);
    }
    rectangle(bounds, enabled ? color : sf::Color(27, 36, 46));
    text(label, bounds.left + 9, bounds.top + 6, 14,
         enabled ? sf::Color(230, 239, 246) : sf::Color(105, 119, 134));
}
float SfmlView::cellSize(const World &world) const
{
    return std::min(layout::GRID_WIDTH / world.getColumns(), layout::GRID_HEIGHT / world.getRows());
}
void SfmlView::drawWorld(const World &world)
{
    float size = cellSize(world);
    for (int row = 0; row < world.getRows(); row++)
    {
        for (int column = 0; column < world.getColumns(); column++)
        {
            float x = layout::GRID_X + column * size;
            float y = layout::GRID_Y + row * size;
            rectangle(sf::FloatRect(x, y, size - 1, size - 1), sf::Color(27, 40, 54));
            int id = world.getCell(row, column);
            if (id == 0)
            {
                continue;
            }
            const SpeciesBase &species = world.getSpecies(id);
            unsigned int rgb = species.getColor();
            sf::Color color(static_cast<sf::Uint8>(rgb >> 16), static_cast<sf::Uint8>(rgb >> 8),
                            static_cast<sf::Uint8>(rgb));
            Shape shape = species.getShape();
            if (shape == Shape::Square)
            {
                rectangle(sf::FloatRect(x + size * .16f, y + size * .16f, size * .65f, size * .65f), color);
            }
            else
            {
                unsigned int points = layout::CIRCLE_POINTS;
                if (shape == Shape::Triangle)
                {
                    points = layout::TRIANGLE_POINTS;
                }
                else if (shape == Shape::Diamond)
                {
                    points = layout::DIAMOND_POINTS;
                }
                sf::CircleShape symbol(size * .38f, points);
                symbol.setOrigin(size * .38f, size * .38f);
                symbol.setPosition(x + size * .48f, y + size * .48f);
                symbol.setFillColor(color);
                window.draw(symbol);
            }
            if (size >= 20)
            {
                text(std::string(1, species.getSymbol()), x + size * .29f, y + size * .12f,
                     static_cast<unsigned int>(size * .55f), sf::Color(12, 24, 35));
            }
        }
    }
    text("Izquierdo: pintar | Derecho: borrar | Arrastra para dibujar", 30, 742, 13);
}
void SfmlView::drawTop(const Laboratory &laboratory)
{
    text("ENJAMBRE / LABORATORIO", 30, 22, 25, ACCENT);
    text("Mundos independientes. Historial acotado y controles gráficos.", 30, 54, 13);
    bool active = laboratory.getActive() != nullptr;
    button(Control::Create, "Nuevo");
    button(Control::Duplicate, "Duplicar", false, active);
    button(Control::Delete, "Eliminar", false, active);
    button(Control::PreviousWorld, "< Mundo", false, laboratory.getCount() > 1);
    button(Control::NextWorld, "Mundo >", false, laboratory.getCount() > 1);
    text("Mundos: " + std::to_string(laboratory.getCount()) + "/" + std::to_string(MAX_WORLDS), 605, 91, 14);
    button(Control::NewLab, "Nuevo laboratorio");
}
void SfmlView::drawInformation(const Laboratory &laboratory, const Timeline &timeline)
{
    const World &world = timeline.getWorld();
    text("#" + std::to_string(laboratory.getActiveId()) + " " + world.getName(), 30, 173, 20);
    text(std::to_string(world.getRows()) + " x " + std::to_string(world.getColumns()), 665, 180, 14);
    rectangle(sf::FloatRect(810, 130, 440, 635), PANEL);
    text("ESPECIE ACTIVA / REGLAS", 830, 142, 16, ACCENT);
    for (int id = 1; id <= world.getSpeciesCount(); id++)
    {
        button(layout::offset(Control::SpeciesFirst, id - 1),
               std::string(1, world.getSpecies(id).getSymbol()), laboratory.getSelectedSpecies() == id);
    }
    text(world.getSpecies(laboratory.getSelectedSpecies()).getName(), 1090, 179, 14);
    text("Generación: " + std::to_string(world.getGeneration()), 830, 218, 16);
    text("Celdas vivas: " + std::to_string(world.getPopulation(0)), 830, 246, 15);
    std::string population;
    for (int id = 1; id <= world.getSpeciesCount(); id++)
    {
        population += std::string(1, world.getSpecies(id).getSymbol()) + ": " +
                      std::to_string(world.getPopulation(id)) + "   ";
    }
    text(population, 830, 274, 14);
}
void SfmlView::drawRules(const Laboratory &laboratory, const World &world)
{
    int id = laboratory.getSelectedSpecies();
    text("Nacimiento: total de vecinos vivos", 830, 310, 14);
    text("Supervivencia: total de vecinos vivos", 830, 382, 14);
    for (int neighbors = 0; neighbors < RULE_COUNT; neighbors++)
    {
        bool enabled = world.isMoore() || neighbors <= 4;
        button(layout::offset(Control::BirthFirst, neighbors), std::to_string(neighbors),
               world.getSpecies(id).getRule(true, neighbors), enabled);
        button(layout::offset(Control::SurvivalFirst, neighbors), std::to_string(neighbors),
               world.getSpecies(id).getRule(false, neighbors), enabled);
    }
    text("Resaltado = regla habilitada. Clic para alternar.", 830, 450, 12);
}
void SfmlView::drawSettings(const Laboratory &laboratory, const World &world)
{
    button(Control::Neighborhood, world.isMoore() ? "Moore (8)" : "Von Neumann (4)");
    button(Control::Border, world.isWrapped() ? "Borde: toroidal" : "Borde: cerrado");
    text("Velocidad: " + std::to_string(laboratory.getSpeed()) + " gen/s", 830, 526, 14);
    button(Control::SpeedMinus, "-");
    button(Control::SpeedPlus, "+");
    button(Control::Mutation, world.isMutationEnabled() ? "Mutación: SÍ" : "Mutación: NO",
           world.isMutationEnabled());
    text(std::to_string(world.getMutationPercent()) + " %", 1010, 573, 15);
    button(Control::MutationMinus, "-");
    button(Control::MutationPlus, "+");
    text("Densidad: " + std::to_string(laboratory.getDensity()) + " %", 830, 621, 14);
    button(Control::DensityMinus, "-");
    button(Control::DensityPlus, "+");
    text("W cuadrado | S círculo | G triángulo | Q rombo", 830, 668, 12);
}
void SfmlView::drawTime(const Laboratory &laboratory, const Timeline &timeline)
{
    text("Historial: " + std::to_string(timeline.getHistoryCount()) + "/" +
             std::to_string(timeline.getHistoryLimit()) +
             " | Cursor: " + std::to_string(timeline.getCursorPosition() + 1),
         830, 704, 13);
    text("Estados antiguos liberados: " + std::to_string(timeline.getDiscardedPast()), 830, 730, 12);
    button(Control::Backward, "< Atrás", false, timeline.hasPrevious());
    button(Control::Forward, "Adelante >");
    button(Control::Play, laboratory.isPlaying() ? "Pausar" : "Reproducir", laboratory.isPlaying());
    button(Control::Clear, "Limpiar");
    button(Control::Random, "Aleatorio");
    button(Control::Pattern, patternMode ? "Patrón: clic..." : "Patrón 3x3", patternMode);
}
void SfmlView::drawModal()
{
    rectangle(sf::FloatRect(0, 0, layout::WIDTH, layout::HEIGHT), sf::Color(0, 0, 0, 185));
    rectangle(sf::FloatRect(390, 180, 500, 470), PANEL);
    if (modal == Modal::Create)
    {
        text("Crear un mundo", 420, 202, 24, ACCENT);
        const std::string labels[FORM_FIELD_COUNT] = {
            "Nombre (máx. " + std::to_string(MAX_NAME_LENGTH) + " bytes)",
            "Filas (" + std::to_string(configuration.getMinRows()) + "–" +
                std::to_string(configuration.getMaxRows()) + ")",
            "Columnas (" + std::to_string(configuration.getMinColumns()) + "–" +
                std::to_string(configuration.getMaxColumns()) + ")",
            "Especies (3–" + std::to_string(configuration.getAvailableSpecies()) + ")"};
        for (int index = 0; index < FORM_FIELD_COUNT; index++)
        {
            sf::FloatRect field = layout::fieldBounds(index);
            text(labels[index], 420, field.top + 7, 14);
            rectangle(field, focusedField == index ? sf::Color(44, 84, 92) : CONTROL);
            text(fields[index] + (focusedField == index ? "|" : ""), field.left + 9, field.top + 7, 13);
        }
        text("Tab: siguiente campo | Ctrl+A: reemplazar texto", 420, 485, 12);
    }
    else if (modal == Modal::Save || modal == Modal::Load)
    {
        text(modal == Modal::Save ? "Guardar laboratorio" : "Cargar laboratorio", 420, 202, 24, ACCENT);
        wrappedText(
            modal == Modal::Save
                ? "Se guardarán todos los mundos y el historial retenido. Se reemplazará el archivo si ya "
                  "existe."
                : "Al cargar se reemplazará el laboratorio abierto. Guarda primero si quieres conservarlo.",
            420, 265, 53);
        rectangle(layout::bounds(Control::FileField), CONTROL);
        text(fileName + "|", 430, 378, 14);
        text("Nombre .txt sin carpetas. Ctrl+A para reemplazar.", 420, 425, 13);
    }
    else
    {
        text(modal == Modal::Delete ? "¿Eliminar el mundo activo?" : "¿Iniciar un laboratorio nuevo?", 420,
             220, 22, ACCENT);
        wrappedText("Se eliminarán los estados abiertos de la memoria. Los archivos guardados se "
                    "conservarán. Esta acción no se puede deshacer.",
                    420, 300, 50);
    }
    button(Control::Confirm, modal == Modal::Save   ? "Guardar"
                             : modal == Modal::Load ? "Cargar"
                                                    : "Confirmar");
    button(Control::Cancel, "Cancelar");
    wrappedText(notice, 420, 580, 55, 12);
}
void SfmlView::draw(Laboratory &laboratory)
{
    window.clear(BACKGROUND);
    drawTop(laboratory);
    const Timeline *timeline = laboratory.getActive();
    if (timeline != nullptr)
    {
        drawWorld(timeline->getWorld());
        drawInformation(laboratory, *timeline);
        drawRules(laboratory, timeline->getWorld());
        drawSettings(laboratory, timeline->getWorld());
        drawTime(laboratory, *timeline);
    }
    else
    {
        text("No hay mundos. Usa Nuevo mundo o Cargar.", 30, 210, 20);
    }
    wrappedText(message, 30, 835);
    text("Espacio: pausa | Flechas: tiempo | 1–4: especie | Tab: mundo", 30, 875, 12);
    if (modal != Modal::None)
    {
        drawModal();
    }
    if (!fontReady)
    {
        window.setTitle("Falta assets/DejaVuSans.ttf");
    }
    window.display();
}
void SfmlView::stop(Laboratory &laboratory)
{
    laboratory.setPlaying(false);
    leftDown = false;
    rightDown = false;
    playbackClock.restart();
}
void SfmlView::changed(Laboratory &laboratory, const std::string &detail)
{
    Timeline *timeline = laboratory.getActive();
    if (timeline == nullptr)
    {
        return;
    }
    bool hadFuture = timeline->hasNext();
    timeline->discardFuture();
    stop(laboratory);
    message = hadFuture ? "Futuro descartado. " + detail : detail;
}
void SfmlView::openModal(Modal value, Laboratory &laboratory)
{
    resumeAfterModal = laboratory.isPlaying();
    stop(laboratory);
    modal = value;
    patternMode = false;
    focusedField = 0;
    replaceInput = false;
    notice.clear();
}
void SfmlView::cancelModal(Laboratory &laboratory)
{
    modal = Modal::None;
    laboratory.setPlaying(resumeAfterModal);
    playbackClock.restart();
}
bool SfmlView::parseInteger(const std::string &value, int &result)
{
    result = 0;
    std::stringstream input(value);
    return static_cast<bool>(input >> result) && (input >> std::ws).eof();
}
void SfmlView::submitCreation(Laboratory &laboratory)
{
    WorldSize size{};
    int count = 0;
    if (!parseInteger(fields[1], size.rows) || !parseInteger(fields[2], size.columns) ||
        !parseInteger(fields[3], count) || !laboratory.create(fields[0], size, count))
    {
        notice = "Datos inválidos o límite de mundos alcanzado. Revisa los rangos del formulario.";
        return;
    }
    modal = Modal::None;
    laboratory.selectSpecies(1);
    message = "Mundo creado con las reglas predeterminadas del INI.";
}
bool SfmlView::validFileName() const
{
    if (fileName.size() <= 4 || fileName.size() > MAX_FILE_NAME ||
        fileName.substr(fileName.size() - 4) != ".txt")
    {
        return false;
    }
    for (unsigned char character : fileName)
    {
        if (!((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
              (character >= '0' && character <= '9') || character == '_' || character == '-' ||
              character == '.'))
        {
            return false;
        }
    }
    return fileName.find("..") == std::string::npos;
}
void SfmlView::confirmModal(Laboratory &laboratory)
{
    if (modal == Modal::Create)
    {
        submitCreation(laboratory);
        return;
    }
    if (modal == Modal::Delete)
    {
        laboratory.deleteActive();
        modal = Modal::None;
        message = "Mundo eliminado.";
        return;
    }
    if (modal == Modal::NewLab)
    {
        laboratory.clear();
        laboratory.setSpeed(configuration.getDefaultSpeed());
        laboratory.setDensity(configuration.getDefaultDensity());
        modal = Modal::None;
        message = "Laboratorio nuevo. Usa Nuevo mundo para comenzar.";
        return;
    }
}

void SfmlView::paint(Laboratory &laboratory, bool erase)
{
    Timeline *timeline = laboratory.getActive();
    if (timeline == nullptr || modal != Modal::None)
    {
        return;
    }
    World &world = timeline->editWorld();
    float size = cellSize(world);
    sf::FloatRect grid(layout::GRID_X, layout::GRID_Y, world.getColumns() * size, world.getRows() * size);
    if (!grid.contains(pointer))
    {
        return;
    }
    int row = static_cast<int>((pointer.y - layout::GRID_Y) / size);
    int column = static_cast<int>((pointer.x - layout::GRID_X) / size);
    if (patternMode && !erase)
    {
        if (world.seedPattern(row, column))
        {
            patternMode = false;
            changed(laboratory, "Patrón colocado.");
        }
        else
        {
            message = "El patrón 3x3 no cabe aquí.";
        }
    }
    else if (world.setCell(row, column, erase ? 0 : laboratory.getSelectedSpecies()))
    {
        bool left = leftDown;
        bool right = rightDown;
        changed(laboratory, erase ? "Celda borrada." : "Celda pintada.");
        leftDown = left;
        rightDown = right;
    }
}
bool SfmlView::handleTop(Laboratory &laboratory)
{
    if (over(Control::Create))
    {
        openModal(Modal::Create, laboratory);
    }
    else if (over(Control::NewLab))
    {
        openModal(Modal::NewLab, laboratory);
    }

    else if (over(Control::Duplicate))
    {
        stop(laboratory);
        message =
            laboratory.duplicateActive() ? "Copia independiente creada." : "No se pudo duplicar el mundo.";
    }
    else if (over(Control::Delete) && laboratory.getActive() != nullptr)
    {
        openModal(Modal::Delete, laboratory);
    }
    else if (over(Control::PreviousWorld) || over(Control::NextWorld))
    {
        stop(laboratory);
        patternMode = false;
        if (over(Control::PreviousWorld))
        {
            laboratory.selectPrevious();
        }
        else
        {
            laboratory.selectNext();
        }
    }
    else
    {
        return false;
    }
    return true;
}
bool SfmlView::handleRules(Laboratory &laboratory)
{
    World &world = laboratory.getActive()->editWorld();
    for (int id = 1; id <= world.getSpeciesCount(); id++)
    {
        if (over(layout::offset(Control::SpeciesFirst, id - 1)))
        {
            laboratory.selectSpecies(id);
            return true;
        }
    }
    for (int neighbors = 0; neighbors < RULE_COUNT; neighbors++)
    {
        if (!world.isMoore() && neighbors > 4)
        {
            continue;
        }
        bool birth = over(layout::offset(Control::BirthFirst, neighbors));
        bool survival = over(layout::offset(Control::SurvivalFirst, neighbors));
        if (birth || survival)
        {
            world.toggleRule(laboratory.getSelectedSpecies(), birth, neighbors);
            changed(laboratory, "Regla actualizada.");
            return true;
        }
    }
    return false;
}
bool SfmlView::handleSettings(Laboratory &laboratory)
{
    World &world = laboratory.getActive()->editWorld();
    if (over(Control::Neighborhood))
    {
        world.toggleNeighborhood();
        changed(laboratory, "Vecindario actualizado.");
    }
    else if (over(Control::Border))
    {
        world.toggleBorder();
        changed(laboratory, "Borde actualizado.");
    }
    else if (over(Control::Mutation))
    {
        world.toggleMutation();
        changed(laboratory, "Mutación actualizada.");
    }
    else if (over(Control::SpeedMinus) || over(Control::SpeedPlus))
    {
        laboratory.setSpeed(laboratory.getSpeed() + (over(Control::SpeedMinus) ? -1 : 1));
        message = "Velocidad actualizada; el historial se conserva.";
    }
    else if (over(Control::DensityMinus) || over(Control::DensityPlus))
    {
        laboratory.setDensity(std::max(
            0, std::min(PERCENT_BASE, laboratory.getDensity() +
                                          (over(Control::DensityMinus) ? -PERCENT_STEP : PERCENT_STEP))));
        message = "Densidad actualizada; el historial se conserva.";
    }
    else if (over(Control::MutationMinus) || over(Control::MutationPlus))
    {
        int value = std::max(
            0, std::min(PERCENT_BASE, world.getMutationPercent() +
                                          (over(Control::MutationMinus) ? -PERCENT_STEP : PERCENT_STEP)));
        if (world.setMutationPercent(value))
        {
            changed(laboratory, "Probabilidad actualizada.");
        }
    }
    else
    {
        return false;
    }
    return true;
}
void SfmlView::stepBackward(Laboratory &laboratory)
{
    stop(laboratory);
    patternMode = false;
    bool restored = laboratory.getActive()->backward();
    message = restored ? "Estado anterior restaurado." : "Ya estás en el primer estado retenido.";
}
void SfmlView::stepForward(Laboratory &laboratory)
{
    stop(laboratory);
    patternMode = false;
    Timeline &timeline = *laboratory.getActive();
    long long previous = timeline.getDiscardedPast();
    bool advanced = timeline.forward();
    message = advanced ? "Generación avanzada." : "Se alcanzó el límite de generaciones.";
    if (timeline.getDiscardedPast() > previous)
    {
        message += " Se liberó el estado más antiguo.";
    }
}
bool SfmlView::handleTime(Laboratory &laboratory)
{
    World &world = laboratory.getActive()->editWorld();
    if (over(Control::Backward))
    {
        stepBackward(laboratory);
    }
    else if (over(Control::Forward))
    {
        stepForward(laboratory);
    }
    else if (over(Control::Play))
    {
        laboratory.setPlaying(!laboratory.isPlaying());
        patternMode = false;
        playbackClock.restart();
    }
    else if (over(Control::Clear))
    {
        world.clear();
        changed(laboratory, "Mundo limpio; conserva su generación.");
    }
    else if (over(Control::Random))
    {
        world.seedRandom(laboratory.getDensity());
        changed(laboratory, "Siembra aleatoria aplicada.");
    }
    else if (over(Control::Pattern))
    {
        stop(laboratory);
        patternMode = !patternMode;
        message = "Patrón: .W. / ..S / GGG. Haz clic en el tablero.";
    }
    else
    {
        return false;
    }
    return true;
}
void SfmlView::click(Laboratory &laboratory)
{
    if (modal != Modal::None)
    {
        if (over(Control::Cancel))
        {
            cancelModal(laboratory);
        }
        else if (over(Control::Confirm))
        {
            confirmModal(laboratory);
        }
        else if (modal == Modal::Create)
        {
            for (int index = 0; index < FORM_FIELD_COUNT; index++)
            {
                if (layout::fieldBounds(index).contains(pointer))
                {
                    focusedField = index;
                    replaceInput = false;
                }
            }
        }
        return;
    }
    if (handleTop(laboratory) || laboratory.getActive() == nullptr)
    {
        return;
    }
    if (handleRules(laboratory) || handleSettings(laboratory) || handleTime(laboratory))
    {
        return;
    }
    leftDown = true;
    paint(laboratory, false);
}
void SfmlView::processInput(sf::Uint32 character)
{
    bool creation = modal == Modal::Create;
    if (!creation && modal != Modal::Save && modal != Modal::Load)
    {
        return;
    }
    std::string &value = creation ? fields[focusedField] : fileName;
    if (character == 8)
    {
        if (replaceInput)
        {
            value.clear();
        }
        else
        {
            removeCharacter(value);
        }
        replaceInput = false;
        return;
    }
    if (character < 32 || character == 127)
    {
        return;
    }
    if (creation && focusedField > 0 && (character < '0' || character > '9'))
    {
        return;
    }
    if (!creation && character > 126)
    {
        return;
    }
    std::string encoded;
    if (character < 128)
    {
        encoded += static_cast<char>(character);
    }
    else if (character <= 0x7FF)
    {
        encoded += static_cast<char>(0xC0 | (character >> 6));
        encoded += static_cast<char>(0x80 | (character & 0x3F));
    }
    else
    {
        return;
    }
    if (replaceInput)
    {
        value.clear();
        replaceInput = false;
    }
    std::size_t limit = creation ? (focusedField == 0 ? MAX_NAME_LENGTH : 3) : MAX_FILE_NAME;
    if (value.size() + encoded.size() <= limit)
    {
        value += encoded;
    }
}
void SfmlView::processKey(const sf::Event::KeyEvent &key, Laboratory &laboratory)
{
    if (modal != Modal::None)
    {
        if (key.code == sf::Keyboard::Escape)
        {
            cancelModal(laboratory);
        }
        else if (key.code == sf::Keyboard::Return)
        {
            confirmModal(laboratory);
        }
        else if (key.code == sf::Keyboard::Tab && modal == Modal::Create)
        {
            focusedField = (focusedField + 1) % FORM_FIELD_COUNT;
            replaceInput = false;
        }
        else if (key.control && key.code == sf::Keyboard::A)
        {
            replaceInput = true;
        }
        return;
    }

    if (key.code == sf::Keyboard::N)
    {
        openModal(Modal::Create, laboratory);
        return;
    }
    if (laboratory.getActive() == nullptr)
    {
        return;
    }
    if (key.code == sf::Keyboard::Space)
    {
        laboratory.setPlaying(!laboratory.isPlaying());
        patternMode = false;
        playbackClock.restart();
    }
    else if (key.code == sf::Keyboard::Left)
    {
        stepBackward(laboratory);
    }
    else if (key.code == sf::Keyboard::Right)
    {
        stepForward(laboratory);
    }
    else if (key.code == sf::Keyboard::Tab)
    {
        stop(laboratory);
        laboratory.selectNext();
        patternMode = false;
    }
    else if (key.code >= sf::Keyboard::Num1 && key.code <= sf::Keyboard::Num4)
    {
        laboratory.selectSpecies(key.code - sf::Keyboard::Num1 + 1);
    }
    else if (key.code == sf::Keyboard::Escape)
    {
        stop(laboratory);
        patternMode = false;
    }
}
void SfmlView::processEvent(const sf::Event &event, Laboratory &laboratory)
{
    if (event.type == sf::Event::Closed)
    {
        window.close();
        return;
    }
    if (event.type == sf::Event::Resized)
    {
        window.setView(sf::View(sf::FloatRect(0, 0, layout::WIDTH, layout::HEIGHT)));
    }
    if (event.type == sf::Event::LostFocus)
    {
        stop(laboratory);
    }
    if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            leftDown = false;
        }
        if (event.mouseButton.button == sf::Mouse::Right)
        {
            rightDown = false;
        }
    }
    if (event.type == sf::Event::MouseButtonPressed)
    {
        pointer = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            click(laboratory);
        }
        else if (event.mouseButton.button == sf::Mouse::Right && modal == Modal::None)
        {
            rightDown = true;
            paint(laboratory, true);
        }
    }
    if (event.type == sf::Event::MouseMoved)
    {
        pointer = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        if (leftDown)
        {
            paint(laboratory, false);
        }
        else if (rightDown)
        {
            paint(laboratory, true);
        }
    }
    if (event.type == sf::Event::TextEntered)
    {
        processInput(event.text.unicode);
    }
    if (event.type == sf::Event::KeyPressed)
    {
        processKey(event.key, laboratory);
    }
}
void SfmlView::run(Laboratory &laboratory)
{
    while (window.isOpen())
    {
        try
        {
            sf::Event event{};
            while (window.pollEvent(event))
            {
                processEvent(event, laboratory);
            }
            if (!window.isOpen())
            {
                break;
            }
            Timeline *active = laboratory.getActive();
            if (laboratory.isPlaying() && modal == Modal::None && active != nullptr &&
                playbackClock.getElapsedTime().asSeconds() >= 1.0f / laboratory.getSpeed())
            {
                if (!active->forward())
                {
                    stop(laboratory);
                    message = "Límite de generaciones alcanzado.";
                }
                playbackClock.restart();
            }
            pointer = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }
        catch (const std::exception &error)
        {
            stop(laboratory);
            notice = "No se completó la operación: " + std::string(error.what());
            message = notice;
        }
        draw(laboratory);
    }
}
