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
void require(bool condition, const std::string &message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
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
