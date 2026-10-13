#include "world.h"

#include <iomanip>
#include <iostream>
#include <cstdlib>

using namespace std;

namespace
{
const int PATTERN_SIZE = 3;

const char* const SPECIES_NAMES[MAX_SPECIES] =
{
    "Trabajador", "Explorador", "Guardián", "Reina"
};


const char SPECIES_SYMBOLS[MAX_SPECIES + 1] = { '.', 'W', 'S', 'G', 'Q' };

int toIndex(Species species)
{
    return static_cast<int>(species) - 1;
}

Species fromIndex(int index)
{
    return static_cast<Species>(index + 1);
}

char symbolOf(Species species)
{
    return SPECIES_SYMBOLS[static_cast<int>(species)];
}

void printRuleList(const RuleList& rules)
{
    for (int i = 0; i < rules.getCount(); i++)
    {
        cout << rules.getValue(i);

        if (i < rules.getCount() - 1)
        {
            cout << ", ";
        }
    }
}
}

Cell** World::createMatrix(int rowCount, int columnCount)
{
    Cell** matrix = new Cell * [rowCount];

    for (int i = 0; i < rowCount; i++)
    {
        matrix[i] = new Cell[columnCount];
    }

    return matrix;
}

void World::deleteMatrix(Cell** matrix, int rowCount)
{
    for (int i = 0; i < rowCount; i++)
    {
        delete[] matrix[i];
    }

    delete[] matrix;
}


void World::copyFrom(const World& other)
{
    rows = other.rows;
    columns = other.columns;
    speciesCount = other.speciesCount;
    generation = other.generation;

    birthRuleList = other.birthRuleList;

    cellMatrix = nullptr;
    speciesRuleList = nullptr;

    if (speciesCount > 0)
    {
        speciesRuleList = new SpeciesRules[speciesCount];

        for (int i = 0; i < speciesCount; i++)
        {
            speciesRuleList[i] = other.speciesRuleList[i];
        }
    }

    if (other.cellMatrix != nullptr)
    {
        cellMatrix = createMatrix(rows, columns);

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                cellMatrix[i][j] = other.cellMatrix[i][j];
            }
        }
    }
}

World::World()
{
    rows = 0;
    columns = 0;
    speciesCount = 0;
    generation = 0;

    cellMatrix = nullptr;
    speciesRuleList = nullptr;
}

World::World(WorldSize size, int newSpeciesCount)
{
    rows = 0;
    columns = 0;
    speciesCount = 0;
    generation = 0;

    cellMatrix = nullptr;
    speciesRuleList = nullptr;

    configure(size, newSpeciesCount);
}

World::World(const World& other)
{
    copyFrom(other);
}

World::~World()
{
    clearCells();

    delete[] speciesRuleList;
}

World& World::operator=(const World& other)
{
    if (this != &other)
    {
        clearCells();

        delete[] speciesRuleList;

        copyFrom(other);
    }

    return *this;
}

bool World::configure(WorldSize size, int newSpeciesCount)
{
    bool validSize = size.getRows() >= MIN_SIZE && size.getRows() <= MAX_SIZE
        && size.getColumns() >= MIN_SIZE && size.getColumns() <= MAX_SIZE;

    bool validSpecies = newSpeciesCount >= MIN_SPECIES && newSpeciesCount <= MAX_SPECIES;

    if (!validSize || !validSpecies)
    {
        return false;
    }

    clearCells();

    delete[] speciesRuleList;

    rows = size.getRows();
    columns = size.getColumns();
    speciesCount = newSpeciesCount;

    generation = 0;

    cellMatrix = createMatrix(rows, columns);

    speciesRuleList = new SpeciesRules[speciesCount];

    setDefaultRules();

    return true;
}

void World::setDefaultRules()
{
    int birthValues[2] = { 3, 6 };
    int survivalValues[2] = { 2, 3 };

    birthRuleList = RuleList(birthValues, 2);

    for (int i = 0; i < speciesCount; i++)
    {
        speciesRuleList[i] = SpeciesRules(SPECIES_NAMES[i], RuleList(survivalValues, 2));
    }
}

void World::clearCells()
{
    if (cellMatrix != nullptr)
    {
        deleteMatrix(cellMatrix, rows);

        cellMatrix = nullptr;
    }
}

bool World::isInside(int row, int column) const
{
    return row >= 0 && row < rows && column >= 0 && column < columns;
}

int World::countNeighbors(int row, int column) const
{
    int count = 0;

    for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
    {
        for (int columnOffset = -1; columnOffset <= 1; columnOffset++)
        {
            int neighborRow = row + rowOffset;
            int neighborColumn = column + columnOffset;
            bool isSelf = rowOffset == 0 && columnOffset == 0;

            if (!isSelf && isInside(neighborRow, neighborColumn)
                && cellMatrix[neighborRow][neighborColumn].isAlive())
            {
                count++;
            }
        }
    }

    return count;
}

Species World::getDominantSpecies(int row, int column) const
{
    int speciesCountList[MAX_SPECIES] = { 0 };

    for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
    {
        for (int columnOffset = -1; columnOffset <= 1; columnOffset++)
        {
            int neighborRow = row + rowOffset;
            int neighborColumn = column + columnOffset;

            if (isInside(neighborRow, neighborColumn))
            {
                int index = toIndex(cellMatrix[neighborRow][neighborColumn].getSpecies());

                if (index >= 0 && index < speciesCount)
                {
                    speciesCountList[index]++;
                }
            }
        }
    }

    
    int dominantIndex = 0;

    for (int i = 1; i < speciesCount; i++)
    {
        if (speciesCountList[i] > speciesCountList[dominantIndex])
        {
            dominantIndex = i;
        }
    }

    return fromIndex(dominantIndex);
}

bool World::canBeBorn(int neighbors) const
{
    return birthRuleList.contains(neighbors);
}

bool World::seedManual(Position position, Species species)
{
    int index = toIndex(species);

    if (!isInside(position.getRow(), position.getColumn()) || index < 0 || index >= speciesCount)
    {
        return false;
    }

    cellMatrix[position.getRow()][position.getColumn()].setSpecies(species);

    return true;
}

void World::seedRandom(double density)
{
    if (cellMatrix == nullptr)
    {
        return;
    }

    if (density < 0.0)
    {
        density = 0.0;
    }

    if (density > 1.0)
    {
        density = 1.0;
    }

    emptyCells();

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            int randomValue = rand() % PERCENT_BASE;

            if (randomValue < density * PERCENT_BASE)
            {
                cellMatrix[i][j].setSpecies(fromIndex(rand() % speciesCount));
            }
        }
    }
}

bool World::seedPattern(Position start)
{
    Species pattern[PATTERN_SIZE][PATTERN_SIZE] =
    {
        { Species::Empty,    Species::Worker,   Species::Empty },
        { Species::Empty,    Species::Empty,    Species::Scout },
        { Species::Guardian, Species::Guardian, Species::Guardian }
    };

    bool fits = isInside(start.getRow(), start.getColumn())
        && isInside(start.getRow() + PATTERN_SIZE - 1, start.getColumn() + PATTERN_SIZE - 1);

    if (!fits)
    {
        return false;
    }

    emptyCells();

    for (int i = 0; i < PATTERN_SIZE; i++)
    {
        for (int j = 0; j < PATTERN_SIZE; j++)
        {
            cellMatrix[start.getRow() + i][start.getColumn() + j].setSpecies(pattern[i][j]);
        }
    }

    return true;
}

void World::evolve()
{
    if (cellMatrix == nullptr)
    {
        return;
    }

    Cell** nextMatrix = createMatrix(rows, columns);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            Species currentSpecies = cellMatrix[i][j].getSpecies();

            int neighbors = countNeighbors(i, j);

            if (currentSpecies == Species::Empty)
            {
                if (canBeBorn(neighbors))
                {
                    nextMatrix[i][j].setSpecies(getDominantSpecies(i, j));
                }
            }
            else if (speciesRuleList[toIndex(currentSpecies)].survives(neighbors))
            {
                nextMatrix[i][j].setSpecies(currentSpecies);
            }
        }
    }

    deleteMatrix(cellMatrix, rows);

    cellMatrix = nextMatrix;

    generation++;
}

void World::emptyCells()
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            cellMatrix[i][j].setSpecies(Species::Empty);
        }
    }
}

void World::clearWorld()
{
    if (cellMatrix == nullptr)
    {
        return;
    }

    emptyCells();

    generation = 0;
}

void World::display() const
{
    if (cellMatrix == nullptr)
    {
        cout << "El mundo no está configurado." << endl;
        return;
    }

    cout << endl;

    cout << "Generación: " << generation << endl;

    cout << endl;

   
    int columnWidth = 2;

    if (columns >= 10)
    {
        columnWidth = 3;
    }

    cout << "   ";

    for (int j = 0; j < columns; j++)
    {
        cout << setw(columnWidth) << j + 1;
    }

    cout << endl;

    for (int i = 0; i < rows; i++)
    {
        cout << setw(2) << i + 1 << " ";

        for (int j = 0; j < columns; j++)
        {
            cout << setw(columnWidth) << symbolOf(cellMatrix[i][j].getSpecies());
        }

        cout << endl;
    }

    cout << endl;

    for (int i = 0; i < speciesCount; i++)
    {
        cout << SPECIES_SYMBOLS[i + 1] << " = " << speciesRuleList[i].getName() << endl;
    }

    cout << ". = Vacía" << endl;
}

void World::showStatistics() const
{
    if (cellMatrix == nullptr)
    {
        cout << "El mundo no está configurado." << endl;
        return;
    }

    int populationList[MAX_SPECIES] = { 0 };

    int totalAlive = 0;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            int index = toIndex(cellMatrix[i][j].getSpecies());

            if (index >= 0)
            {
                populationList[index]++;

                totalAlive++;
            }
        }
    }

    cout << endl;

    cout << "========== ESTADÍSTICAS ==========" << endl;

    cout << "Generación: " << generation << endl;

    cout << "Total de celdas vivas: " << totalAlive << endl;

    for (int i = 0; i < speciesCount; i++)
    {
        cout << "E" << i + 1 << " - " << speciesRuleList[i].getName() << ": " << populationList[i] << endl;
    }

    cout << "==================================" << endl;
}

void World::showRules() const
{
    cout << endl;

    cout << "============= REGLAS =============" << endl;

    cout << "Nacimiento: ";

    printRuleList(birthRuleList);

    cout << endl;

    for (int i = 0; i < speciesCount; i++)
    {
        cout << "E" << i + 1 << " - " << speciesRuleList[i].getName() << " supervivencia: ";

        printRuleList(speciesRuleList[i].getSurvivalRules());

        cout << endl;
    }

    cout << "Vecindario: Moore de 8 vecinos" << endl;

    cout << "Bordes: cerrados" << endl;

    cout << "Desempate: menor índice" << endl;

    cout << "==================================" << endl;
}

bool World::setBirthRules(const RuleList& rules)
{
    if (!rules.isValid())
    {
        return false;
    }

    birthRuleList = rules;

    return true;
}

bool World::setSpeciesSurvivalRules(Species species, const RuleList& rules)
{
    int index = toIndex(species);

    if (index < 0 || index >= speciesCount)
    {
        return false;
    }

    return speciesRuleList[index].setSurvivalRules(rules);
}

int World::getRows() const
{
    return rows;
}

int World::getColumns() const
{
    return columns;
}

int World::getSpeciesCount() const
{
    return speciesCount;
}

int World::getGeneration() const
{
    return generation;
}

bool World::isConfigured() const
{
    return cellMatrix != nullptr;
}
