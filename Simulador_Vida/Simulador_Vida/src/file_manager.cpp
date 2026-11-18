#include "file_manager.h"
#include <cstdio>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
namespace
{
constexpr int FORMAT_VERSION = 1;
constexpr int MAX_LINE_LENGTH = 4096;
constexpr int MAX_FILE_BYTES = 24000000;
void require(bool condition, const std::string &message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}
std::stringstream record(std::istream &input, const std::string &expected, int &lineNumber)
{
    std::string line;
    require(static_cast<bool>(std::getline(input, line)), "Archivo incompleto: falta " + expected + ".");
    lineNumber++;
    require(line.size() <= MAX_LINE_LENGTH, "Línea demasiado larga.");
    if (!line.empty() && line.back() == '\r')
    {
        line.pop_back();
    }
    std::stringstream fields(line);
    std::string tag;
    fields >> tag;
    require(tag == expected, "Se esperaba " + expected + ", se encontró " + tag + ".");
    return fields;
}
void endRecord(std::stringstream &fields)
{
    fields >> std::ws;
    require(fields.eof() && !fields.bad(), "Sobran datos en un registro.");
}
void marker(std::istream &input, const std::string &tag, int &lineNumber)
{
    std::stringstream fields = record(input, tag, lineNumber);
    endRecord(fields);
}
} // namespace
void FileManager::writeWorld(std::ostream &output, const World &world)
{
    output << "STATE " << world.generation << ' ' << world.size.rows << ' ' << world.size.columns << ' '
           << world.speciesCount << ' ' << world.moore << ' ' << world.wrapped << ' ' << world.mutationEnabled
           << ' ' << world.mutationPercent << ' ' << world.randomState << '\n';
    output << "NAME " << world.name << '\n';
    for (int id = 1; id <= world.speciesCount; id++)
    {
        int birthMask = 0;
        int survivalMask = 0;
        for (int neighbors = 0; neighbors < RULE_COUNT; neighbors++)
        {
            if (world.getSpecies(id).getRule(true, neighbors))
            {
                birthMask |= 1 << neighbors;
            }
            if (world.getSpecies(id).getRule(false, neighbors))
            {
                survivalMask |= 1 << neighbors;
            }
        }
        output << "RULE " << id << ' ' << birthMask << ' ' << survivalMask << '\n';
    }
    for (int row = 0; row < world.size.rows; row++)
    {
        output << "ROW";
        for (int column = 0; column < world.size.columns; column++)
        {
            output << ' ' << world.getCell(row, column);
        }
        output << '\n';
    }
    output << "END_STATE\n";
}
World FileManager::readWorld(std::istream &input, int &lineNumber, long long &cellsRead)
{
    int generation = 0;
    WorldSize size{};
    int speciesCount = 0;
    int moore = 0;
    int wrapped = 0;
    int mutationEnabled = 0;
    int mutationPercent = 0;
    unsigned long long randomState = 0;
    std::stringstream fields = record(input, "STATE", lineNumber);
    require(static_cast<bool>(fields >> generation >> size.rows >> size.columns >> speciesCount >> moore >>
                              wrapped >> mutationEnabled >> mutationPercent >> randomState),
            "Estado inválido.");
    endRecord(fields);
    require(generation >= 0 && generation <= MAX_GENERATION && (moore == 0 || moore == 1) &&
                (wrapped == 0 || wrapped == 1) && (mutationEnabled == 0 || mutationEnabled == 1) &&
                mutationPercent >= 0 && mutationPercent <= PERCENT_BASE &&
                randomState <= std::numeric_limits<std::uint32_t>::max(),
            "Valores de estado fuera de rango.");
    std::stringstream nameLine = record(input, "NAME", lineNumber);
    std::string name;
    require(nameLine.get() == ' ', "Nombre ausente.");
    std::getline(nameLine, name);
    require(World::validConfiguration(name, size, speciesCount), "Nombre, dimensiones o especies inválidos.");
    cellsRead += static_cast<long long>(size.rows) * size.columns;
    require(cellsRead <= MAX_SAVED_CELLS, "El guardado supera el límite de celdas históricas.");
    World world(name, size, speciesCount);
    world.generation = generation;
    world.moore = moore == 1;
    world.wrapped = wrapped == 1;
    world.mutationEnabled = mutationEnabled == 1;
    world.mutationPercent = mutationPercent;
    world.randomState = static_cast<std::uint32_t>(randomState);
    for (int expectedId = 1; expectedId <= speciesCount; expectedId++)
    {
        int id = 0;
        int birthMask = 0;
        int survivalMask = 0;
        std::stringstream rules = record(input, "RULE", lineNumber);
        require(static_cast<bool>(rules >> id >> birthMask >> survivalMask), "Reglas incompletas.");
        endRecord(rules);
        const int maxMask = (1 << RULE_COUNT) - 1;
        require(id == expectedId && birthMask >= 0 && birthMask <= maxMask && survivalMask >= 0 &&
                    survivalMask <= maxMask,
                "Regla o índice de especie inválido.");
        bool birth[RULE_COUNT] = {};
        bool survival[RULE_COUNT] = {};
        for (int neighbors = 0; neighbors < RULE_COUNT; neighbors++)
        {
            birth[neighbors] = (birthMask & (1 << neighbors)) != 0;
            survival[neighbors] = (survivalMask & (1 << neighbors)) != 0;
        }
        world.setRules(id, birth, survival);
    }
    for (int row = 0; row < size.rows; row++)
    {
        std::stringstream cells = record(input, "ROW", lineNumber);
        for (int column = 0; column < size.columns; column++)
        {
            int value = 0;
            require(static_cast<bool>(cells >> value) && value >= 0 && value <= speciesCount,
                    "Celda inválida o fila incompleta.");
            world.cellMatrix[row][column] = static_cast<SpeciesId>(value);
        }
        endRecord(cells);
    }
    marker(input, "END_STATE", lineNumber);
    return world;
}
void FileManager::writeLaboratory(std::ostream &output, const Laboratory &laboratory)
{
    long long totalCells = 0;
    int totalStates = 0;
    for (Laboratory::WorldNode *node = laboratory.first; node != nullptr; node = node->next)
    {
        totalStates += node->history.nodeCount;
        totalCells += static_cast<long long>(node->history.nodeCount) * node->history.getWorld().getRows() *
                      node->history.getWorld().getColumns();
    }
    require(totalStates <= MAX_SAVED_STATES && totalCells <= MAX_SAVED_CELLS,
            "Demasiados estados para un guardado seguro. Elimine mundos o reduzca el historial.");
    output << "ENJAMBRE_LAB " << FORMAT_VERSION << '\n';
    output << "LAB " << laboratory.count << ' ' << laboratory.getActiveId() << ' ' << laboratory.nextId << ' '
           << laboratory.speed << ' ' << laboratory.density << ' ' << laboratory.selectedSpecies << ' '
           << laboratory.playing << '\n';
    for (Laboratory::WorldNode *node = laboratory.first; node != nullptr; node = node->next)
    {
        const Timeline &history = node->history;
        output << "WORLD " << node->id << ' ' << history.nodeCount << ' ' << history.getCursorPosition()
               << ' ' << history.historyLimit << ' ' << history.discardedPast << '\n';
        for (Timeline::HistoryNode *state = history.first; state != nullptr; state = state->next)
        {
            writeWorld(output, state->state);
        }
        output << "END_WORLD\n";
    }
    output << "END_LAB\n";
}
void FileManager::readLaboratory(std::istream &input, Laboratory &laboratory)
{
    int lineNumber = 0;
    try
    {
        int version = 0;
        std::stringstream header = record(input, "ENJAMBRE_LAB", lineNumber);
        require(static_cast<bool>(header >> version) && version == FORMAT_VERSION,
                "Versión de guardado incompatible.");
        endRecord(header);
        int count = 0;
        int activeId = 0;
        int nextId = 0;
        int speed = 0;
        int density = 0;
        int selected = 0;
        int playing = 0;
        std::stringstream metadata = record(input, "LAB", lineNumber);
        require(static_cast<bool>(metadata >> count >> activeId >> nextId >> speed >> density >> selected >>
                                  playing),
                "Cabecera incompleta.");
        endRecord(metadata);
        require(count >= 0 && count <= MAX_WORLDS && nextId > 0 && nextId <= MAX_GENERATION &&
                    speed >= MIN_SPEED && speed <= MAX_SPEED && density >= 0 && density <= PERCENT_BASE &&
                    selected >= 1 && selected <= MAX_SPECIES && (playing == 0 || playing == 1),
                "Opciones de laboratorio inválidas.");
        long long totalCells = 0;
        int totalStates = 0;
        int highestId = 0;
        Laboratory::WorldNode *selectedNode = nullptr;
        for (int index = 0; index < count; index++)
        {
            int id = 0;
            int states = 0;
            int cursor = 0;
            int limit = 0;
            long long discarded = 0;
            std::stringstream worldHeader = record(input, "WORLD", lineNumber);
            require(static_cast<bool>(worldHeader >> id >> states >> cursor >> limit >> discarded),
                    "Historial incompleto.");
            endRecord(worldHeader);
            totalStates += states > 0 && states <= MAX_HISTORY_LIMIT ? states : 0;
            require(id > highestId && id < nextId && states >= 1 && limit >= 2 &&
                        limit <= MAX_HISTORY_LIMIT && states <= limit && cursor >= 0 && cursor < states &&
                        discarded >= 0 && discarded <= MAX_GENERATION && totalStates <= MAX_SAVED_STATES,
                    "Metadatos históricos inválidos.");
            highestId = id;
            World firstWorld = readWorld(input, lineNumber, totalCells);
            Laboratory::WorldNode *node = new Laboratory::WorldNode(id, firstWorld, limit);
            laboratory.append(node);
            node->history.discardedPast = discarded;
            if (id == activeId)
            {
                selectedNode = node;
            }
            for (int stateIndex = 1; stateIndex < states; stateIndex++)
            {
                World nextWorld = readWorld(input, lineNumber, totalCells);
                const World &previous = node->history.getWorld();
                require(nextWorld.getRows() == firstWorld.getRows() &&
                            nextWorld.getColumns() == firstWorld.getColumns() &&
                            nextWorld.getSpeciesCount() == firstWorld.getSpeciesCount() &&
                            nextWorld.getName() == firstWorld.getName() &&
                            nextWorld.getGeneration() == previous.getGeneration() + 1,
                        "La secuencia histórica es incoherente.");
                node->history.appendSnapshot(nextWorld);
            }
            while (node->history.getCursorPosition() > cursor)
            {
                node->history.backward();
            }
            marker(input, "END_WORLD", lineNumber);
        }
        require((count == 0 && activeId == 0 && playing == 0 && selected == 1) ||
                    (count > 0 && selectedNode != nullptr &&
                     selected <= selectedNode->history.getWorld().getSpeciesCount()),
                "Mundo activo inválido.");
        marker(input, "END_LAB", lineNumber);
        std::string trailing;
        while (std::getline(input, trailing))
        {
            require(trailing.find_first_not_of(" \t\r") == std::string::npos,
                    "Hay contenido después de END_LAB.");
        }
        require(!input.bad(), "Error al leer el guardado.");
        laboratory.active = selectedNode;
        laboratory.nextId = nextId;
        laboratory.speed = speed;
        laboratory.density = density;
        laboratory.selectedSpecies = selected;
        laboratory.playing = playing == 1;
    }
    catch (const std::exception &error)
    {
        throw std::runtime_error("Línea " + std::to_string(lineNumber) + ": " + error.what());
    }
}
bool FileManager::save(const Laboratory &laboratory, const std::string &path, std::string &message)
{
    std::string temporary = path + ".tmp";
    try
    {
        std::ofstream output(temporary, std::ios::trunc);
        require(static_cast<bool>(output), "No se pudo abrir el archivo temporal para escribir.");
        writeLaboratory(output, laboratory);
        output.flush();
        require(static_cast<bool>(output), "No se pudo completar la escritura.");
        output.close();
        require(!output.fail(), "No se pudo cerrar el archivo de guardado.");
#ifdef _WIN32
        bool replaced = MoveFileExA(temporary.c_str(), path.c_str(),
                                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
        bool replaced = std::rename(temporary.c_str(), path.c_str()) == 0;
#endif
        require(replaced, "No se pudo reemplazar el guardado; el archivo anterior se conserva.");
        message = "Laboratorio guardado en " + path + ".";
        return true;
    }
    catch (const std::exception &error)
    {
        std::remove(temporary.c_str());
        message = "No se guardó: " + std::string(error.what());
        return false;
    }
}
bool FileManager::load(Laboratory &laboratory, const std::string &path, std::string &message)
{
    try
    {
        std::ifstream input(path, std::ios::ate);
        require(static_cast<bool>(input), "El archivo no existe o no se puede abrir.");
        std::streamoff bytes = input.tellg();
        require(bytes >= 0 && bytes <= MAX_FILE_BYTES, "Tamaño de guardado inválido o excesivo.");
        input.seekg(0);
        Laboratory candidate(laboratory.configuration);
        readLaboratory(input, candidate);
        laboratory.swap(candidate);
        message = "Laboratorio cargado desde " + path + ".";
        return true;
    }
    catch (const std::exception &error)
    {
        message = "No se cargó: " + std::string(error.what());
        return false;
    }
}
