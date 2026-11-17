#include "configuration.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
Configuration::Configuration()
    : minRows(5), maxRows(50), minColumns(5), maxColumns(50), defaultSize{20, 25},
      availableSpecies(MAX_SPECIES), defaultSpecies(MIN_SPECIES), defaultSpeed(5), defaultDensity(30),
      historyLimit(DEFAULT_HISTORY_LIMIT), defaultMoore(true), defaultWrapped(false), birth{}, survival{}
{
    for (int id = 0; id < MAX_SPECIES; id++)
    {
        birth[id][3] = true;
        birth[id][6] = true;
        survival[id][2] = true;
        survival[id][3] = true;
    }
}
bool Configuration::valid() const
{
    return minRows >= MIN_SIZE && minRows <= maxRows && maxRows <= MAX_SIZE && minColumns >= MIN_SIZE &&
           minColumns <= maxColumns && maxColumns <= MAX_SIZE && defaultSize.rows >= minRows &&
           defaultSize.rows <= maxRows && defaultSize.columns >= minColumns &&
           defaultSize.columns <= maxColumns && availableSpecies >= MIN_SPECIES &&
           availableSpecies <= MAX_SPECIES && defaultSpecies >= MIN_SPECIES &&
           defaultSpecies <= availableSpecies && defaultSpeed >= MIN_SPEED && defaultSpeed <= MAX_SPEED &&
           defaultDensity >= 0 && defaultDensity <= PERCENT_BASE && historyLimit >= 2 &&
           historyLimit <= MAX_HISTORY_LIMIT;
}

bool Configuration::allows(WorldSize size, int count) const
{
    return size.rows >= minRows && size.rows <= maxRows && size.columns >= minColumns &&
           size.columns <= maxColumns && count >= MIN_SPECIES && count <= availableSpecies;
}
void Configuration::applyTo(World &world) const
{
    for (int id = 1; id <= world.getSpeciesCount(); id++)
    {
        world.setRules(id, birth[id - 1], survival[id - 1]);
    }
    if (world.isMoore() != defaultMoore)
    {
        world.toggleNeighborhood();
    }
    if (world.isWrapped() != defaultWrapped)
    {
        world.toggleBorder();
    }
}
WorldSize Configuration::getDefaultSize() const
{
    return defaultSize;
}
int Configuration::getDefaultSpecies() const
{
    return defaultSpecies;
}
int Configuration::getAvailableSpecies() const
{
    return availableSpecies;
}
int Configuration::getDefaultSpeed() const
{
    return defaultSpeed;
}
int Configuration::getDefaultDensity() const
{
    return defaultDensity;
}
int Configuration::getHistoryLimit() const
{
    return historyLimit;
}
int Configuration::getMinRows() const
{
    return minRows;
}
int Configuration::getMaxRows() const
{
    return maxRows;
}
int Configuration::getMinColumns() const
{
    return minColumns;
}
int Configuration::getMaxColumns() const
{
    return maxColumns;
}
