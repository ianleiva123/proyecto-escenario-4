#pragma once
#include "cell.h"
#include "rule_list.h"
#include "species_rules.h"
#include "world_size.h"
#include "position.h"

const int MIN_SIZE = 5;
const int MAX_SIZE = 50;
const int MIN_SPECIES = 3;
const int MAX_SPECIES = 4;
const int PERCENT_BASE = 100;

class World
{
private:
    int rows;
    int columns;
    int speciesCount;
    int generation;

    Cell** cellMatrix;

    RuleList birthRuleList;

    SpeciesRules* speciesRuleList;

    static Cell** createMatrix(int rowCount, int columnCount);
    static void deleteMatrix(Cell** matrix, int rowCount);

    void copyFrom(const World& other);

    void setDefaultRules();

    bool isInside(int row, int column) const;

    int countNeighbors(int row, int column) const;

    Species getDominantSpecies(int row, int column) const;

    bool canBeBorn(int neighbors) const;

    void emptyCells();

    void clearCells();

public:
    World();

    World(WorldSize size, int newSpeciesCount);

    World(const World& other);

    ~World();

    World& operator=(const World& other);

    bool configure(WorldSize size, int newSpeciesCount);

    bool seedManual(Position position, Species species);

    void seedRandom(double density);

    bool seedPattern(Position start);

    void evolve();

    void clearWorld();

    void display() const;

    void showStatistics() const;

    void showRules() const;

    bool setBirthRules(const RuleList& rules);

    bool setSpeciesSurvivalRules(Species species, const RuleList& rules);

    int getRows() const;
    int getColumns() const;
    int getSpeciesCount() const;
    int getGeneration() const;

    bool isConfigured() const;
};
