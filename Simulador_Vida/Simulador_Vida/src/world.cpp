#include "world.h"
#include <cstdlib>
#include <stdexcept>
#include <utility>
World::World(const string& newName, int newRows, int newColumns, int newSpeciesCount)
    : name(newName), rows(newRows), columns(newColumns), speciesCount(newSpeciesCount), generation(0),
      cells(nullptr), moore(true), wrapped(false), speed(5), density(30)
{
    if (!validConfiguration(name, rows, columns, speciesCount)) throw invalid_argument("Configuracion invalida");
    for (int i = 0; i < 4; i++) species[i] = nullptr;
    species[0] = new Worker(); species[1] = new Scout(); species[2] = new Guardian();
    if (speciesCount == 4) species[3] = new Queen();
    cells = new int*[rows];
    for (int r = 0; r < rows; r++) cells[r] = new int[columns]();
}
bool World::validConfiguration(const string& name, int rows, int columns, int speciesCount)
{
    return !name.empty() && name.size() <= 24 && rows >= 5 && rows <= 50 && columns >= 5 && columns <= 50
        && speciesCount >= 3 && speciesCount <= 4;
}
void World::release()
{
    if (cells != nullptr) { for (int r = 0; r < rows; r++) delete[] cells[r]; delete[] cells; }
    for (int i = 0; i < 4; i++) { delete species[i]; species[i] = nullptr; }
    cells = nullptr;
}
void World::copyFrom(const World& other)
{
    name = other.name; rows = other.rows; columns = other.columns; speciesCount = other.speciesCount;
    generation = other.generation; moore = other.moore; wrapped = other.wrapped; speed = other.speed;
    density = other.density;
    for (int i = 0; i < 4; i++) species[i] = other.species[i] == nullptr ? nullptr : other.species[i]->clone();
    cells = new int*[rows];
    for (int r = 0; r < rows; r++)
    {
        cells[r] = new int[columns];
        for (int c = 0; c < columns; c++) cells[r][c] = other.cells[r][c];
    }
}
World::World(const World& other) { copyFrom(other); }
World::~World() { release(); }
World& World::operator=(const World& other)
{
    if (this != &other) { World copy(other); 
        using std::swap;
        swap(name, copy.name); swap(rows, copy.rows); swap(columns, copy.columns); swap(speciesCount, copy.speciesCount);
        swap(generation, copy.generation); swap(cells, copy.cells);
        for (int i = 0; i < 4; i++) swap(species[i], copy.species[i]);
        swap(moore, copy.moore); swap(wrapped, copy.wrapped); swap(speed, copy.speed);
        swap(density, copy.density);
    }
    return *this;
}
const string& World::getName() const { return name; }
void World::setName(const string& value) { if (!value.empty() && value.size() <= 24) name = value; }
int World::getRows() const { return rows; }
int World::getColumns() const { return columns; }
int World::getSpeciesCount() const { return speciesCount; }
int World::getGeneration() const { return generation; }
int World::getCell(int r, int c) const { return r >= 0 && r < rows && c >= 0 && c < columns ? cells[r][c] : 0; }
bool World::setCell(int r, int c, int value)
{
    if (r < 0 || r >= rows || c < 0 || c >= columns || value < 0 || value > speciesCount || cells[r][c] == value) return false;
    cells[r][c] = value; return true;
}
const SpeciesBase& World::getSpecies(int id) const
{
    if (id < 1 || id > speciesCount) throw out_of_range("Especie invalida");
    return *species[id - 1];
}
bool World::toggleRule(int id, bool isBirth, int neighbors)
{
    return id >= 1 && id <= speciesCount && species[id - 1]->toggleRule(isBirth, neighbors);
}
bool World::isMoore() const { return moore; }
bool World::isWrapped() const { return wrapped; }
void World::toggleNeighborhood() { moore = !moore; }
void World::toggleBorder() { wrapped = !wrapped; }
int World::getSpeed() const { return speed; }
bool World::setSpeed(int value) { if (value < 1 || value > 20 || value == speed) return false; speed = value; return true; }
int World::getDensity() const { return density; }
bool World::setDensity(int value) { if (value < 0 || value > 100 || value == density) return false; density = value; return true; }
int World::getPopulation(int id) const
{
    int count = 0;
    for (int r = 0; r < rows; r++) for (int c = 0; c < columns; c++)
        if (id == 0 ? cells[r][c] != 0 : cells[r][c] == id) count++;
    return count;
}
void World::clear() { for (int r = 0; r < rows; r++) for (int c = 0; c < columns; c++) cells[r][c] = 0; }
void World::seedRandom()
{
    clear();
    for (int r = 0; r < rows; r++) for (int c = 0; c < columns; c++)
        if (rand() % 100 < density) cells[r][c] = 1 + rand() % speciesCount;
}
bool World::seedPattern(int row, int column)
{
    if (row < 0 || column < 0 || row + 2 >= rows || column + 2 >= columns) return false;
    const int pattern[3][3] = { {0,1,0}, {0,0,2}, {3,3,3} };
 
    for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++) cells[row+r][column+c] = pattern[r][c];
    return true;
}
void World::countNeighbors(int row, int column, int counts[4], int& total) const
{
    total = 0;
    for (int i = 0; i < 4; i++) counts[i] = 0;
    for (int dr = -1; dr <= 1; dr++) for (int dc = -1; dc <= 1; dc++)
    {
        if ((dr == 0 && dc == 0) || (!moore && dr != 0 && dc != 0)) continue;
        int r = row + dr, c = column + dc;
        if (wrapped) { r = (r + rows) % rows; c = (c + columns) % columns; }
        int value = getCell(r, c);
        if (value != 0) { total++; counts[value - 1]++; }
    }
}
void World::evolve()
{
    int** next = new int*[rows];
    for (int r = 0; r < rows; r++) next[r] = new int[columns]();
    for (int r = 0; r < rows; r++) for (int c = 0; c < columns; c++)
    {
        int counts[4], total;
        countNeighbors(r, c, counts, total);
        int current = cells[r][c];
        if (current == 0)
        {
            int best = 0;
            
            for (int i = 1; i < speciesCount; i++) if (counts[i] > counts[best]) best = i;
            if (species[best]->canBeBorn(total)) next[r][c] = best + 1;
        }
        else if (species[current - 1]->survives(total))
        {
            next[r][c] = current;

        }
    }
    for (int r = 0; r < rows; r++) delete[] cells[r];
    delete[] cells; cells = next; generation++;
}
