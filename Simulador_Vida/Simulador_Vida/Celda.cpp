#include "Celda.h"

Cell::Cell()
{
    species = 0;
}

Cell::Cell(int species)
{
    this->species = species;
}

int Cell::getSpecies() const
{
    return species;
}

void Cell::setSpecies(int species)
{
    this->species = species;
}

bool Cell::isAlive() const
{
    return species != 0;
}