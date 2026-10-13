#pragma once

enum class Species
{
    Empty,
    Worker,
    Scout,
    Guardian,
    Queen
};

class Cell
{
private:
    Species species;

public:
    Cell();
    Cell(Species newSpecies);

    Species getSpecies() const;
    void setSpecies(Species newSpecies);
    bool isAlive() const;
};
