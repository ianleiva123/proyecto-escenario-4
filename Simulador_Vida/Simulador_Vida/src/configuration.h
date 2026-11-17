#pragma once
#include "world.h"
class Configuration
{
  private:
    int minRows;
    int maxRows;
    int minColumns;
    int maxColumns;
    WorldSize defaultSize;
    int availableSpecies;
    int defaultSpecies;
    int defaultSpeed;
    int defaultDensity;
    int historyLimit;
    bool defaultMoore;
    bool defaultWrapped;
    bool birth[MAX_SPECIES][RULE_COUNT];
    bool survival[MAX_SPECIES][RULE_COUNT];
    bool valid() const;

  public:
    Configuration();
    bool load(const std::string &path, std::string &message);
    bool allows(WorldSize size, int speciesCount) const;
    void applyTo(World &world) const;
    WorldSize getDefaultSize() const;
    int getDefaultSpecies() const;
    int getAvailableSpecies() const;
    int getDefaultSpeed() const;
    int getDefaultDensity() const;
    int getHistoryLimit() const;
    int getMinRows() const;
    int getMaxRows() const;
    int getMinColumns() const;
    int getMaxColumns() const;
};
