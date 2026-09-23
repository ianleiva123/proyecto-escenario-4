#include "Mundo.h"

#include <iostream>
#include <cstdlib>

using namespace std;

World::World()
{
    rows = 0;
    columns = 0;
    speciesCount = 0;
    generation = 0;

    cells = nullptr;

    birthRules = nullptr;
    birthCount = 0;

    speciesRules = nullptr;
}

World::World( int rows, int columns, int speciesCount)
{
    this->rows = 0;
    this->columns = 0;
    this->speciesCount = 0;
    generation = 0;

    cells = nullptr;

    birthRules = nullptr;
    birthCount = 0;

    speciesRules = nullptr;

    configure(rows,columns,speciesCount);
}

World::World(const World& other)
{
    rows = other.rows;
    columns = other.columns;
    speciesCount = other.speciesCount;
    generation = other.generation;

    birthCount = other.birthCount;

    cells = nullptr;
    birthRules = nullptr;
    speciesRules = nullptr;

    if (birthCount > 0)
    {
        birthRules = new int[birthCount];

        for (int i = 0; i < birthCount; i++)
        {
            birthRules[i] = other.birthRules[i];
        }
    }

    if (speciesCount > 0)
    {
        speciesRules =
            new SpeciesRules[speciesCount];

        for (int i = 0; i < speciesCount; i++)
        {
            speciesRules[i] = other.speciesRules[i];
        }
    }

    if (rows > 0 && columns > 0)
    {
        cells = new Cell * [rows];

        for (int i = 0; i < rows; i++)
        {
            cells[i] = new Cell[columns];

            for (int j = 0; j < columns; j++)
            {
                cells[i][j] = other.cells[i][j];
            }
        }
    }
}

World::~World()
{
    clearCells();

    delete[] birthRules;
    delete[] speciesRules;
}

World& World::operator=( const World& other)
{
    if (this != &other)
    {
        clearCells();

        delete[] birthRules;
        delete[] speciesRules;

        rows = other.rows;
        columns = other.columns;
        speciesCount = other.speciesCount;
        generation = other.generation;

        birthCount = other.birthCount;

        cells = nullptr;
        birthRules = nullptr;
        speciesRules = nullptr;

        if (birthCount > 0)
        {
            birthRules = new int[birthCount];

            for (int i = 0; i < birthCount; i++)
            {
                birthRules[i] = other.birthRules[i];
            }
        }

        if (speciesCount > 0)
        {
            speciesRules = new SpeciesRules[speciesCount];

            for (int i = 0; i < speciesCount; i++)
            {
                speciesRules[i] = other.speciesRules[i];
            }
        }

        if (rows > 0 && columns > 0)
        {
            cells = new Cell * [rows];

            for (int i = 0;i < rows; i++)
            {
                cells[i] = new Cell[columns];

                for (int j = 0;j < columns;j++)
                {
                    cells[i][j] = other.cells[i][j];
                }
            }
        }
    }

    return *this;
}

bool World::configure( int rows,int columns,int speciesCount)
{
    // [REVISION] BP-V3: 5, 50 y 3 son numeros magicos; declare MIN_SIZE, MAX_SIZE y SPECIES_COUNT como constantes con nombre.
    if (rows < 5 || rows > 50 || columns < 5 || columns > 50 || speciesCount != 3)
    {
        return false;
    }

    clearCells();

    delete[] birthRules;
    delete[] speciesRules;

    birthRules = nullptr;
    speciesRules = nullptr;

    this->rows = rows;
    this->columns = columns;
    this->speciesCount = speciesCount;

    generation = 0;

    cells = new Cell * [rows];

    for (int i = 0; i < rows; i++)
    {
        cells[i] = new Cell[columns];
    }

    speciesRules = new SpeciesRules[speciesCount];

    setDefaultRules();

    return true;
}

void World::setDefaultRules()
{
    delete[] birthRules;

    birthCount = 2;

    birthRules = new int[birthCount];

    birthRules[0] = 3;
    birthRules[1] = 6;

    int survivalRules[2] =
    {
        2, 3
    };

    speciesRules[0] =
        SpeciesRules(  1, "Trabajador", survivalRules, 2);

    speciesRules[1] =
        SpeciesRules(2, "Explorador",survivalRules,2);

    speciesRules[2] = SpeciesRules(3,"Guardian",survivalRules,2);
}

void World::clearCells()
{
    if (cells != nullptr)
    {
        for (int i = 0; i < rows; i++)
        {
            delete[] cells[i];
        }

        delete[] cells;

        cells = nullptr;
    }
}

bool World::isInside( int row, int column) const
{
    return row >= 0 && row < rows && column >= 0 && column < columns;
}

// [NOTA] Esta funcion tiene 29 lineas y llega a 4 niveles de anidacion, el maximo que permite la guia.
// Se puede bajar la anidacion extrayendo el cuerpo del ciclo interno a una funcion aparte, o usando
// continue y salidas tempranas en vez de anidar condicionales.
// Sugerencia, no afecta la nota.
int World::countNeighbors( int row, int column) const
{
    int count = 0;

    for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
    {
        for (int columnOffset = -1; columnOffset <= 1; columnOffset++)
        {
            if (rowOffset == 0 && columnOffset == 0)
            {
                continue;
            }

            int neighborRow = row + rowOffset;

            int neighborColumn = column + columnOffset;

            if (isInside( neighborRow, neighborColumn))
            {
                if (cells[neighborRow] [neighborColumn].isAlive())
                {
                    count++;
                }
            }
        }
    }

    return count;
}

// [NOTA] Esta funcion tiene 51 lineas y llega a 4 niveles de anidacion, el maximo que permite la guia.
// Conviene partirla en funciones privadas cortas y usar salidas tempranas en vez de anidar condicionales.
// Sugerencia, no afecta la nota.
int World::getDominantSpecies( int row, int column) const
{
    int* speciesCounts = new int[speciesCount];

    for (int i = 0; i < speciesCount; i++)
    {
        speciesCounts[i] = 0;
    }

    for (int rowOffset = -1; rowOffset <= 1;rowOffset++)
    {
        for (int columnOffset = -1; columnOffset <= 1; columnOffset++)
        {
            if (rowOffset == 0 && columnOffset == 0)
            {
                continue;
            }

            int neighborRow = row + rowOffset;

            int neighborColumn = column + columnOffset;

            if (isInside( neighborRow, neighborColumn))
            {
                int species =cells[neighborRow][neighborColumn].getSpecies();

                if (species >= 1 && species <= speciesCount)
                {
                    speciesCounts[species - 1]++;
                }
            }
        }
    }

    // [REVISION] PR-R7/PR-D5: CORRECTO. El ">" estricto conserva el primer maximo, o sea el desempate por menor indice de la Config 4.
    int dominantSpecies = 1;
    int highestCount = speciesCounts[0];

    for (int i = 1;i < speciesCount;i++)
    {
        if (speciesCounts[i] > highestCount)
        {
            highestCount = speciesCounts[i];

            dominantSpecies = i + 1;
        }
    }

    delete[] speciesCounts;

    return dominantSpecies;
}

bool World::canBeBorn(
    int neighbors) const
{
    for (int i = 0; i < birthCount;i++)
    {
        if (birthRules[i] == neighbors)
        {
            return true;
        }
    }

    return false;
}

void World::seedManual(int row,int column, int species)
{
    if (!isInside(row, column))
    {
        return;
    }

    if (species < 1 || species > speciesCount)
    {
        return;
    }

    cells[row][column].setSpecies(species);
}

void World::seedRandom(double density)
{
    if (density < 0.0)
    {
        density = 0.0;
    }

    if (density > 1.0)
    {
        density = 1.0;
    }

    clearWorld();

    for (int i = 0;i < rows;i++)
    {
        for (int j = 0;j < columns;j++)
        {
            int randomValue = rand() % 100;

            if (randomValue <density * 100)
            {
                int species =(rand() %speciesCount) + 1;

                cells[i][j].setSpecies(species);
            }
        }
    }

    generation = 0;
}

// [NOTA] Esta funcion tiene 33 lineas y llega a 4 niveles de anidacion, el maximo que permite la guia.
// Se puede bajar la anidacion extrayendo el cuerpo del ciclo interno a una funcion aparte, o usando
// continue y salidas tempranas en vez de anidar condicionales.
// Sugerencia, no afecta la nota.
void World::seedPattern(int startRow,int startColumn)
{
    int pattern[3][3] =
    {
        { 0, 1, 0 },
        { 0, 0, 1 },
        { 1, 1, 1 }
    };

    // [REVISION] PR-R4: clearWorld() borra toda siembra previa y reinicia la generacion a 0 sin avisar al usuario.
    clearWorld();

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (pattern[i][j] == 1)
            {
                int row =startRow + i;

                int column =startColumn + j;

                if (isInside(row,column))
                {
                    // [REVISION] PR-R4: esta formula NO produce el glider que el menu muestra en pantalla (. W . / . . S / G G G).
                    int species = ((i + j)% speciesCount) + 1;

                    cells[row][column].setSpecies(species);
                }
            }
        }
    }

    generation = 0;
}

// [NOTA] Esta funcion tiene 52 lineas y llega a 4 niveles de anidacion, el maximo que permite la guia.
// Hace tres cosas distintas a la vez: reservar la matriz nueva, decidir el estado de cada celda, y liberar
// la matriz vieja e intercambiarla. Conviene separarla en funciones privadas cortas, por ejemplo una que
// reserve la matriz, una que la libere, y una que reciba fila y columna y devuelva el estado que le toca a
// esa celda. Con eso el cuerpo queda en el doble ciclo llamando a esa ultima, y la decision por celda se
// escribe con clausulas guarda y salida temprana en vez de if/else anidados. Cada pieza se lee y se prueba
// por separado. Sugerencia, no afecta la nota.
void World::evolve()
{
    if (cells == nullptr)
    {
        return;
    }

    // [REVISION] PR-R5: CORRECTO. Doble buffer real: se lee siempre de cells y se escribe en nextCells, que se intercambia al final.
    Cell** nextCells =new Cell * [rows];

    for (int i = 0; i < rows;i++)
    {
        nextCells[i] =new Cell[columns];
    }

    for (int i = 0;i < rows; i++)
    {
        for (int j = 0;j < columns;j++)
        {
            int currentSpecies =cells[i][j].getSpecies();

            int neighbors = countNeighbors(i, j);

            if (currentSpecies == 0)
            {
                if (canBeBorn(neighbors))
                {
                    int newSpecies =getDominantSpecies( i, j);

                    nextCells[i][j].setSpecies(newSpecies);
                }
            }
            else
            {
                if (speciesRules[currentSpecies - 1].survives(neighbors))
                {
                    nextCells[i][j].setSpecies(currentSpecies);
                }
            }
        }
    }

    for (int i = 0;i < rows; i++)
    {
        delete[] cells[i];
    }

    delete[] cells;

    cells = nextCells;

    generation++;
}

void World::clearWorld()
{
    if (cells == nullptr)
    {
        return;
    }

    for (int i = 0; i < rows;i++)
    {
        for (int j = 0;j < columns;j++)
        {
            cells[i][j].setSpecies(0);
        }
    }

    generation = 0;
}

// [NOTA] Esta funcion tiene 65 lineas y hace varias cosas distintas. La guia pide funciones cortas,
// con una sola responsabilidad cada una.
// Conviene partirla extrayendo cada bloque a su propia funcion privada, con un nombre que diga que hace.
// Sugerencia, no afecta la nota.
void World::display() const
{
    if (cells == nullptr)
    {
        cout << "El mundo no esta configurado." << endl;
        return;
    }

    cout << endl;

    cout << "Generacion: "<< generation<< endl;

    cout << endl;

    cout << "    ";

    // [REVISION] PR-R9: con 10+ columnas el encabezado usa 3 caracteres y la rejilla 2; los numeros dejan de alinearse con las celdas.
    for (int j = 0; j < columns; j++)
    {
        cout << j + 1 << " ";
    }

    cout << endl;

    for (int i = 0;i < rows;i++)
    {
        cout << i + 1;

        if (i + 1 < 10)
        {
            cout << " ";
        }

        cout << " ";

        for (int j = 0;j < columns;j++)
        {
            int species =cells[i][j].getSpecies();

            if (species == 0)
            {
                cout << ". ";
            }
            else if (species == 1)
            {
                cout << "W ";
            }
            else if (species == 2)
            {
                cout << "S ";
            }
            else if (species == 3)
            {
                cout << "G ";
            }
        }

        cout << endl;
    }

    cout << endl;

    // [REVISION] BP-U1: la leyenda dice Worker/Scout/Guardian pero las estadisticas dicen Trabajador/Explorador; unifique los nombres en espanol.
    cout << "W = Worker" << endl;
    cout << "S = Scout" << endl;
    cout << "G = Guardian" << endl;
}

// [NOTA] Esta funcion tiene 52 lineas y hace varias cosas distintas. La guia pide funciones cortas,
// con una sola responsabilidad cada una.
// Conviene partirla extrayendo cada bloque a su propia funcion privada, con un nombre que diga que hace.
// Sugerencia, no afecta la nota.
void World::showStatistics() const
{
    if (cells == nullptr)
    {
        cout << "El mundo no esta configurado." << endl;

        return;
    }

    int* population = new int[speciesCount];

    for (int i = 0;i < speciesCount; i++)
    {
        population[i] = 0;
    }

    int totalAlive = 0;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            int species =cells[i][j].getSpecies();

            if (species > 0)
            {
                population[species - 1]++;

                totalAlive++;
            }
        }
    }

    cout << endl;

    cout << "========== ESTADISTICAS ==========" << endl;

    cout << "Generacion: " << generation << endl;

    cout << "Total de celdas vivas: " << totalAlive << endl;

    for (int i = 0;
        i < speciesCount;
        i++)
    {
        cout << "E" << i + 1 << " - " << speciesRules[i].getName() << ": " << population[i] << endl;
    }

    cout << "==================================" << endl;

    delete[] population;
}

// [NOTA] Esta funcion tiene 45 lineas y hace varias cosas distintas. La guia pide funciones cortas,
// con una sola responsabilidad cada una.
// Conviene partirla extrayendo cada bloque a su propia funcion privada, con un nombre que diga que hace.
// Sugerencia, no afecta la nota.
void World::showRules() const
{
    cout << endl;

    cout << "============= REGLAS =============" << endl;

    cout << "Nacimiento: ";

    for (int i = 0;i < birthCount; i++)
    {
        cout << birthRules[i];

        if (i < birthCount - 1)
        {
            cout << ", ";
        }
    }

    cout << endl;

    for (int i = 0; i < speciesCount; i++)
    {
        cout << "E" << i + 1 << " - " << speciesRules[i].getName() << " supervivencia: ";

        for (int j = 0; j < speciesRules[i].getSurvivalCount();j++)
        {
            cout << speciesRules[i].getSurvivalRule(j);

            if (j <speciesRules[i].getSurvivalCount() - 1)
            {
                cout << ", ";
            }
        }

        cout << endl;
    }

    cout << "Vecindario: Moore de 8 vecinos" << endl;

    cout << "Bordes: cerrados" << endl;

    cout << "Desempate: menor indice" << endl;

    cout << "==================================" << endl;
}

// [REVISION] PR-D1: no se valida que cada valor este en 0..8; un 0 de nacimiento llena el mundo entero de E1 en una generacion.
void World::setBirthRules(int* rules,int count)
{
    if (rules == nullptr ||count <= 0)
    {
        return;
    }

    delete[] birthRules;

    birthCount = count;

    birthRules = new int[birthCount];

    for (int i = 0;i < birthCount;i++)
    {
        birthRules[i] =rules[i];
    }
}

void World::setSpeciesSurvivalRules(int species,int* rules,int count)
{
    if (species < 1 ||species > speciesCount ||rules == nullptr ||count <= 0)
    {
        return;
    }

    speciesRules[species - 1].setSurvivalRules(rules,count);
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
    return cells != nullptr;
}