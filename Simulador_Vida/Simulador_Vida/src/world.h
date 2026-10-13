#pragma once
#include <string>
#include "species.h"
using namespace std;
class World
{
private:
    string name;
    int rows, columns, speciesCount, generation;
    int** cells;
    SpeciesBase* species[4];
    bool moore, wrapped;
    int speed, density;
    void release();
    void copyFrom(const World& other);
    void countNeighbors(int row, int column, int counts[4], int& total) const;
public:
    World(const string& newName = "Mundo", int newRows = 20, int newColumns = 25, int newSpeciesCount = 3);
    World(const World& other);
    ~World();
    World& operator=(const World& other);
    static bool validConfiguration(const string& name, int rows, int columns, int speciesCount);
    const string& getName() const;
    void setName(const string& newName);
    int getRows() const;
    int getColumns() const;
    int getSpeciesCount() const;
    int getGeneration() const;
    int getCell(int row, int column) const;
    bool setCell(int row, int column, int value);
    const SpeciesBase& getSpecies(int id) const;
    bool toggleRule(int id, bool isBirth, int neighbors);
    bool isMoore() const;
    bool isWrapped() const;
    void toggleNeighborhood();
    void toggleBorder();
    int getSpeed() const;
    bool setSpeed(int value);
    int getDensity() const;
    bool setDensity(int value);
    int getPopulation(int id) const;
    void clear();
    void seedRandom();
    bool seedPattern(int row, int column);
    void evolve();
};
