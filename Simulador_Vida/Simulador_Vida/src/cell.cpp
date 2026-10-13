#include "cell.h"

Cell::Cell()
{
    species = Species::Empty;
}

Cell::Cell(Species newSpecies)
{
    species = newSpecies;
}

Species Cell::getSpecies() const
{
    return species;
}

void Cell::setSpecies(Species newSpecies)
{
    species = newSpecies;
}

bool Cell::isAlive() const
{
    return species != Species::Empty;
}
