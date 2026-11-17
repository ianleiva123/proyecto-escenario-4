#pragma once
#include "species.h"
#include <string>

class FileManager;
class World
{
    friend class FileManager;

  private:
    std::string name;
    WorldSize size;
    int speciesCount;
    int generation;
    SpeciesId **cellMatrix;
    SpeciesBase *species[MAX_SPECIES];
    bool moore;
    bool wrapped;
    int mutationPercent;
    bool mutationEnabled;
    std::uint32_t randomState;
    static SpeciesId **createMatrix(WorldSize size);
    static void deleteMatrix(SpeciesId **matrix, int rows);
    void release();
    void copyFrom(const World &other);
    void countNeighbors(int row, int column, int counts[MAX_SPECIES], int &total) const;
    unsigned int nextRandom();

  public:
    World(const std::string &name = "Mundo", WorldSize size = WorldSize(), int speciesCount = MIN_SPECIES);
    World(const World &other);
    ~World();
    World &operator=(const World &other);
    void swap(World &other) noexcept;
    static bool validConfiguration(const std::string &name, WorldSize size, int speciesCount);
    const std::string &getName() const;
    void setName(const std::string &name);
    int getRows() const;
    int getColumns() const;
    int getSpeciesCount() const;
    int getGeneration() const;
    int getCell(int row, int column) const;
    bool setCell(int row, int column, int value);
    const SpeciesBase &getSpecies(int id) const;
    bool toggleRule(int id, bool isBirth, int neighbors);
    void setRules(int id, const bool birth[RULE_COUNT], const bool survival[RULE_COUNT]);
    bool isMoore() const;
    bool isWrapped() const;
    void toggleNeighborhood();
    void toggleBorder();
    int getMutationPercent() const;
    bool setMutationPercent(int value);
    bool isMutationEnabled() const;
    void toggleMutation();
    int getPopulation(int id) const;
    void clear();
    void seedRandom(int densityPercent);
    bool seedPattern(int row, int column);
    bool evolve();
};
