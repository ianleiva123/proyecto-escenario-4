#include "world.h"
#include <cstdlib>
#include <stdexcept>
#include <utility>

SpeciesId **World::createMatrix(WorldSize newSize)
{
    SpeciesId **matrix = new SpeciesId *[newSize.rows]();
    try
    {
        for (int row = 0; row < newSize.rows; row++)
        {
            matrix[row] = new SpeciesId[newSize.columns]();
        }
    }
    catch (...)
    {
        deleteMatrix(matrix, newSize.rows);
        throw;
    }
    return matrix;
}
void World::deleteMatrix(SpeciesId **matrix, int rowCount)
{
    if (matrix != nullptr)
    {
        for (int row = 0; row < rowCount; row++)
        {
            delete[] matrix[row];
        }
        delete[] matrix;
    }
}
World::World(const std::string &newName, WorldSize newSize, int newSpeciesCount)
    : name(newName), size(newSize), speciesCount(newSpeciesCount), generation(0), cellMatrix(nullptr),
      species{}, moore(true), wrapped(false), mutationPercent(10), mutationEnabled(false),
      randomState(static_cast<std::uint32_t>(std::rand()) + 1u)
{
    if (!validConfiguration(name, size, speciesCount))
    {
        throw std::invalid_argument("Configuración de mundo inválida.");
    }
    try
    {
        species[0] = new Worker();
        species[1] = new Scout();
        species[2] = new Guardian();
        if (speciesCount == MAX_SPECIES)
        {
            species[MAX_SPECIES - 1] = new Queen();
        }
        cellMatrix = createMatrix(size);
    }
    catch (...)
    {
        release();
        throw;
    }
}
bool World::validConfiguration(const std::string &value, WorldSize dimensions, int count)
{
    bool validName = !value.empty() && value.size() <= MAX_NAME_LENGTH;
    for (unsigned char character : value)
    {
        if (character < 32 || character == 127)
        {
            validName = false;
        }
    }
    return validName && dimensions.rows >= MIN_SIZE && dimensions.rows <= MAX_SIZE &&
           dimensions.columns >= MIN_SIZE && dimensions.columns <= MAX_SIZE && count >= MIN_SPECIES &&
           count <= MAX_SPECIES;
}
void World::release()
{
    deleteMatrix(cellMatrix, size.rows);
    cellMatrix = nullptr;
    for (int index = 0; index < MAX_SPECIES; index++)
    {
        delete species[index];
        species[index] = nullptr;
    }
}
void World::copyFrom(const World &other)
{
    try
    {
        for (int index = 0; index < speciesCount; index++)
        {
            species[index] = other.species[index]->clone();
        }
        cellMatrix = createMatrix(size);
        for (int row = 0; row < size.rows; row++)
        {
            for (int column = 0; column < size.columns; column++)
            {
                cellMatrix[row][column] = other.cellMatrix[row][column];
            }
        }
    }
    catch (...)
    {
        release();
        throw;
    }
}
World::World(const World &other)
    : name(other.name), size(other.size), speciesCount(other.speciesCount), generation(other.generation),
      cellMatrix(nullptr), species{}, moore(other.moore), wrapped(other.wrapped),
      mutationPercent(other.mutationPercent), mutationEnabled(other.mutationEnabled),
      randomState(other.randomState)
{
    copyFrom(other);
}
World::~World()
{
    release();
}
World &World::operator=(const World &other)
{
    if (this != &other)
    {
        World copy(other);
        swap(copy);
    }
    return *this;
}
void World::swap(World &other) noexcept
{
    name.swap(other.name);
    std::swap(size, other.size);
    std::swap(speciesCount, other.speciesCount);
    std::swap(generation, other.generation);
    std::swap(cellMatrix, other.cellMatrix);
    for (int index = 0; index < MAX_SPECIES; index++)
    {
        std::swap(species[index], other.species[index]);
    }
    std::swap(moore, other.moore);
    std::swap(wrapped, other.wrapped);
    std::swap(mutationPercent, other.mutationPercent);
    std::swap(mutationEnabled, other.mutationEnabled);
    std::swap(randomState, other.randomState);
}
const std::string &World::getName() const
{
    return name;
}
void World::setName(const std::string &value)
{
    if (validConfiguration(value, size, speciesCount))
    {
        name = value;
    }
}
int World::getRows() const
{
    return size.rows;
}
int World::getColumns() const
{
    return size.columns;
}
int World::getSpeciesCount() const
{
    return speciesCount;
}
int World::getGeneration() const
{
    return generation;
}
int World::getCell(int row, int column) const
{
    if (row < 0 || row >= size.rows || column < 0 || column >= size.columns)
    {
        return 0;
    }
    return static_cast<int>(cellMatrix[row][column]);
}
bool World::setCell(int row, int column, int value)
{
    if (row < 0 || row >= size.rows || column < 0 || column >= size.columns || value < 0 ||
        value > speciesCount || getCell(row, column) == value)
    {
        return false;
    }
    cellMatrix[row][column] = static_cast<SpeciesId>(value);
    return true;
}
const SpeciesBase &World::getSpecies(int id) const
{
    if (id < 1 || id > speciesCount)
    {
        throw std::out_of_range("Especie inválida.");
    }
    return *species[id - 1];
}
bool World::toggleRule(int id, bool isBirth, int neighbors)
{
    if (id < 1 || id > speciesCount)
    {
        return false;
    }
    return species[id - 1]->toggleRule(isBirth, neighbors);
}
void World::setRules(int id, const bool birth[RULE_COUNT], const bool survival[RULE_COUNT])
{
    if (id >= 1 && id <= speciesCount)
    {
        species[id - 1]->setRules(birth, survival);
    }
}
bool World::isMoore() const
{
    return moore;
}
bool World::isWrapped() const
{
    return wrapped;
}
void World::toggleNeighborhood()
{
    moore = !moore;
}
void World::toggleBorder()
{
    wrapped = !wrapped;
}
int World::getMutationPercent() const
{
    return mutationPercent;
}
bool World::setMutationPercent(int value)
{
    if (value < 0 || value > PERCENT_BASE || value == mutationPercent)
    {
        return false;
    }
    mutationPercent = value;
    return true;
}
bool World::isMutationEnabled() const
{
    return mutationEnabled;
}
void World::toggleMutation()
{
    mutationEnabled = !mutationEnabled;
}
int World::getPopulation(int id) const
{
    int count = 0;
    for (int row = 0; row < size.rows; row++)
    {
        for (int column = 0; column < size.columns; column++)
        {
            int value = getCell(row, column);
            if ((id == 0 && value != 0) || (id > 0 && value == id))
            {
                count++;
            }
        }
    }
    return count;
}
void World::clear()
{
    for (int row = 0; row < size.rows; row++)
    {
        for (int column = 0; column < size.columns; column++)
        {
            cellMatrix[row][column] = SpeciesId::Empty;
        }
    }
}
unsigned int World::nextRandom()
{
    randomState = randomState * 1664525u + 1013904223u;
    return randomState;
}
void World::seedRandom(int densityPercent)
{
    if (densityPercent < 0 || densityPercent > PERCENT_BASE)
    {
        return;
    }
    clear();
    for (int row = 0; row < size.rows; row++)
    {
        for (int column = 0; column < size.columns; column++)
        {
            if (static_cast<int>((nextRandom() >> 8) % PERCENT_BASE) < densityPercent)
            {
                cellMatrix[row][column] = static_cast<SpeciesId>(1 + (nextRandom() >> 8) % speciesCount);
            }
        }
    }
}
bool World::seedPattern(int row, int column)
{
    if (row < 0 || column < 0 || row + PATTERN_SIZE > size.rows || column + PATTERN_SIZE > size.columns)
    {
        return false;
    }
    const SpeciesId pattern[PATTERN_SIZE][PATTERN_SIZE] = {
        {SpeciesId::Empty, SpeciesId::Worker, SpeciesId::Empty},
        {SpeciesId::Empty, SpeciesId::Empty, SpeciesId::Scout},
        {SpeciesId::Guardian, SpeciesId::Guardian, SpeciesId::Guardian}};
    for (int r = 0; r < PATTERN_SIZE; r++)
    {
        for (int c = 0; c < PATTERN_SIZE; c++)
        {
            cellMatrix[row + r][column + c] = pattern[r][c];
        }
    }
    return true;
}
void World::countNeighbors(int row, int column, int counts[MAX_SPECIES], int &total) const
{
    total = 0;
    for (int index = 0; index < MAX_SPECIES; index++)
    {
        counts[index] = 0;
    }
    for (int dr = -1; dr <= 1; dr++)
    {
        for (int dc = -1; dc <= 1; dc++)
        {
            if ((dr == 0 && dc == 0) || (!moore && dr != 0 && dc != 0))
            {
                continue;
            }
            int r = row + dr;
            int c = column + dc;
            if (wrapped)
            {
                r = (r + size.rows) % size.rows;
                c = (c + size.columns) % size.columns;
            }
            if (r == row && c == column)
            {
                continue;
            }
            int value = getCell(r, c);
            if (value != 0)
            {
                total++;
                counts[value - 1]++;
            }
        }
    }
}
bool World::evolve()
{
    if (generation >= MAX_GENERATION)
    {
        return false;
    }
    SpeciesId **nextMatrix = createMatrix(size);
    for (int row = 0; row < size.rows; row++)
    {
        for (int column = 0; column < size.columns; column++)
        {
            int counts[MAX_SPECIES] = {};
            int total = 0;
            countNeighbors(row, column, counts, total);
            int current = getCell(row, column);
            if (current == 0)
            {
                int best = 0;
                for (int index = 1; index < speciesCount; index++)
                {
                    if (counts[index] > counts[best])
                    {
                        best = index;
                    }
                }
                if (species[best]->canBeBorn(total))
                {
                    nextMatrix[row][column] = static_cast<SpeciesId>(best + 1);
                }
            }
            else if (species[current - 1]->survives(total))
            {
                int candidate = current;
                if (mutationEnabled && static_cast<int>((nextRandom() >> 8) % PERCENT_BASE) < mutationPercent)
                {
                    candidate = 1 + (nextRandom() >> 8) % (speciesCount - 1);
                    if (candidate >= current)
                    {
                        candidate++;
                    }
                }
                nextMatrix[row][column] = static_cast<SpeciesId>(candidate);
            }
        }
    }
    deleteMatrix(cellMatrix, size.rows);
    cellMatrix = nextMatrix;
    generation++;
    return true;
}
