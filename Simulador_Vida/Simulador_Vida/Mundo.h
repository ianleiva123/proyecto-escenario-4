#pragma once
#include "Celda.h"
#include "Reglas.h"

using namespace std;

class World
{
private:
    int rows;
    int columns;
    int speciesCount;
    int generation;

    Cell** cells;

    int* birthRules;
    int birthCount;

    SpeciesRules* speciesRules;

    bool isInside(int row, int column) const;

    int countNeighbors(
        int row,
        int column) const;

    int getDominantSpecies(
        int row,
        int column) const;

    bool canBeBorn(int neighbors) const;

    void clearCells();

public:
    World();

    World(
        int rows,
        int columns,
        int speciesCount);

    World(const World& other);

    ~World();

    World& operator=(const World& other);

    bool configure(
        int rows,
        int columns,
        int speciesCount);

    void setDefaultRules();

    void seedManual(
        int row,
        int column,
        int species);

    void seedRandom(double density);

    void seedPattern(
        int startRow,
        int startColumn);

    void evolve();

    void clearWorld();

    void display() const;

    void showStatistics() const;

    void showRules() const;

    void setBirthRules(
        int* rules,
        int count);

    void setSpeciesSurvivalRules(
        int species,
        int* rules,
        int count);

    int getRows() const;
    int getColumns() const;
    int getSpeciesCount() const;
    int getGeneration() const;

    bool isConfigured() const;
};