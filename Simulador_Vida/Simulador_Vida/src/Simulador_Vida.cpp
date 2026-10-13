#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>

#include "world.h"
#include "console_display.h"

using namespace std;

int readInteger(string message)
{
    int value = 0;

    while (true)
    {
        cout << message;

        if (cin >> value)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        if (cin.eof())
        {
            return 0;
        }

        cout << "Entrada inválida." << endl;

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double readDouble(string message)
{
    double value = 0.0;

    while (true)
    {
        cout << message;

        if (cin >> value)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        if (cin.eof())
        {
            return 0.0;
        }

        cout << "Entrada inválida." << endl;

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string rangePrompt(string label, int minimum, int maximum)
{
    return label + " (" + to_string(minimum) + "-" + to_string(maximum) + "): ";
}

bool isWorldReady(const World& world)
{
    if (!world.isConfigured())
    {
        cout << "Primero configure el mundo." << endl;
        return false;
    }

    return true;
}

bool askToContinue()
{
    string answer;

    cout << "Enter para continuar, q para detener: ";

    if (!getline(cin, answer))
    {
        return false;
    }

    return answer != "q" && answer != "Q";
}

void showOfficialConfiguration()
{
    cout << "Configuración oficial asignada:" << endl;
    cout << "Configuración 4 - Enjambre" << endl;
    cout << "Especies: 3 o 4 (se elige al configurar el mundo)" << endl;
    cout << "Nacimiento: 3, 6" << endl;
    cout << "Supervivencia de cada especie: 2, 3" << endl;
    cout << "Desempate: menor índice" << endl;
}

void runWorldConfiguration(World& world)
{
    int rows = readInteger(rangePrompt("Filas", MIN_SIZE, MAX_SIZE));
    int columns = readInteger(rangePrompt("Columnas", MIN_SIZE, MAX_SIZE));

    int speciesCount = readInteger(rangePrompt("Especies", MIN_SPECIES, MAX_SPECIES));

    WorldSize size(rows, columns);

    if (!world.configure(size, speciesCount))
    {
        cout << endl;
        cout << "Configuración inválida." << endl;
        return;
    }

    cout << endl;
    cout << "Mundo configurado correctamente." << endl;
    cout << "Se cargó la configuración oficial." << endl;
}

void runManualSeeding(World& world)
{
    cout << endl;
    cout << "===== SIEMBRA MANUAL =====" << endl;
    cout << "Ingrese 0 en la fila para terminar." << endl;

    while (true)
    {
        int row = readInteger("Fila: ");

        if (row == 0)
        {
            return;
        }

        int column = readInteger("Columna: ");

        int species = readInteger(rangePrompt("Especie", 1, world.getSpeciesCount()));

        // En pantalla las coordenadas empiezan en 1; en el mundo empiezan en 0.
        Position position(row - 1, column - 1);

        if (!world.seedManual(position, static_cast<Species>(species)))
        {
            cout << "Coordenadas o especie inválidas." << endl;
            continue;
        }

        cout << "Organismo colocado." << endl;
    }
}

void runRandomSeeding(World& world)
{
    double density = readDouble("Densidad (0.0 - 1.0): ");

    if (density < 0.0 || density > 1.0)
    {
        cout << "La densidad debe estar entre 0 y 1." << endl;
        return;
    }

    world.seedRandom(density);

    cout << "Mundo sembrado aleatoriamente." << endl;
}

void runPatternPlacement(World& world)
{
    cout << endl;

    cout << "========== GLIDER ==========" << endl;

    cout << ". W ." << endl;
    cout << ". . S" << endl;
    cout << "G G G" << endl;

    cout << endl;

    int row = readInteger("Fila inicial: ");
    int column = readInteger("Columna inicial: ");

    Position start(row - 1, column - 1);

    if (!world.seedPattern(start))
    {
        cout << "El patrón no cabe en esa posición." << endl;
        return;
    }

    cout << "Patrón colocado." << endl;
}

// Muestra el mundo tras cada paso y espera Enter entre pasos para que se pueda leer.
void advanceGenerations(World& world, int count)
{
    for (int i = 0; i < count; i++)
    {
        world.evolve();

        world.display();

        bool isLastStep = i == count - 1;

        if (!isLastStep && !askToContinue())
        {
            return;
        }
    }
}

void runAdvanceMany(World& world)
{
    int generations = readInteger("Cantidad de generaciones: ");

    if (generations <= 0)
    {
        cout << "Debe ser mayor que cero." << endl;
        return;
    }

    advanceGenerations(world, generations);
}


bool readRuleList(string label, RuleList& rules)
{
    const int MAX_COUNT = MAX_NEIGHBORS - MIN_NEIGHBORS + 1;

    int count = 0;

    while (true)
    {
        count = readInteger("Cantidad de valores de " + label + " (1-" + to_string(MAX_COUNT) + "): ");

        if (count >= 1 && count <= MAX_COUNT)
        {
            break;
        }

        if (cin.eof())
        {
            return false;
        }

        cout << "Cantidad inválida." << endl;
    }

    int values[MAX_COUNT];
    int filled = 0;

    while (filled < count)
    {
        int value = readInteger("Valor de " + label + " (0-8): ");

        if (value < MIN_NEIGHBORS || value > MAX_NEIGHBORS)
        {
            cout << "El valor debe estar entre 0 y 8." << endl;
            continue;
        }

        values[filled] = value;
        filled++;
    }

    rules = RuleList(values, count);

    return true;
}


void runRuleConfiguration(World& world)
{
    cout << endl;

    cout << "======= CONFIGURAR REGLAS =======" << endl;

    RuleList birthRules;

    if (!readRuleList("nacimiento", birthRules))
    {
        return;
    }

    RuleList survivalRules[MAX_SPECIES];

    for (int i = 0; i < world.getSpeciesCount(); i++)
    {
        cout << endl;

        cout << "Reglas de supervivencia E" << i + 1 << endl;

        if (!readRuleList("supervivencia", survivalRules[i]))
        {
            return;
        }
    }

    world.setBirthRules(birthRules);

    for (int i = 0; i < world.getSpeciesCount(); i++)
    {
        world.setSpeciesSurvivalRules(static_cast<Species>(i + 1), survivalRules[i]);
    }

    cout << endl;

    cout << "Reglas actualizadas correctamente." << endl;
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    World world;

    ConsoleDisplay display;

    bool running = true;

    display.setupEncoding();

    display.showHeader();

    showOfficialConfiguration();

    while (running)
    {
        display.showMainMenu();

        int option = readInteger("");

        switch (option)
        {
        case 1:
        {
            runWorldConfiguration(world);

            break;
        }

        case 2:
        {
            if (!isWorldReady(world))
            {
                break;
            }

            runManualSeeding(world);

            break;
        }

        case 3:
        {
            if (!isWorldReady(world))
            {
                break;
            }

            runRandomSeeding(world);

            break;
        }

        case 4:
        {
            if (!isWorldReady(world))
            {
                break;
            }

            runPatternPlacement(world);

            break;
        }

        case 5:
        {
            if (!isWorldReady(world))
            {
                break;
            }

            world.display();

            break;
        }

        case 6:
        {
            if (!isWorldReady(world))
            {
                break;
            }

            advanceGenerations(world, 1);

            break;
        }

        case 7:
        {
            if (!isWorldReady(world))
            {
                break;
            }

            runAdvanceMany(world);

            break;
        }

        case 8:
        {
            if (!isWorldReady(world))
            {
                break;
            }

            world.showStatistics();

            break;
        }

        case 9:
        {
            if (!isWorldReady(world))
            {
                break;
            }

            runRuleConfiguration(world);

            break;
        }

        case 10:
        {
            if (!isWorldReady(world))
            {
                break;
            }

            world.showRules();

            break;
        }

        case 0:
        {
            cout << endl;

            cout << "Saliendo del simulador..." << endl;

            running = false;

            break;
        }

        default:
        {
            cout << "Opción inválida." << endl;

            break;
        }
        }
    }

    return 0;
}
