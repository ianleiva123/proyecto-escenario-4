#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include "Mundo.h"
#include "Consola.h"

using namespace std;

const int speciesCount = 3;

int readInteger(string message)
{
    int value;

    while (true)
    {
        cout << message;

        if (cin >> value)
        {
            return value;
        }

        cout << "Entrada invalida."
            << endl;

        cin.clear();
        cin.ignore(10000, '\n');
    }
}

double readDouble(string message)
{
    double value;

    while (true)
    {
        cout << message;

        if (cin >> value)
        {
            return value;
        }

        cout << "Entrada invalida."  << endl;

        cin.clear();
        cin.ignore(10000, '\n');
    }
}

int main()
{
    srand(time(nullptr));

    World world;

    ConsoleDisplay display;

    bool running = true;

    display.showHeader();

    cout << "Configuracion oficial asignada:" << endl;

    cout << "Configuracion 4 - Enjambre" << endl;

    cout << "Especies: 3" << endl;

    cout << "Nacimiento: 3, 6" << endl;

    cout << "E1 supervivencia: 2, 3" << endl;

    cout << "E2 supervivencia: 2, 3" << endl;

    cout << "E3 supervivencia: 2, 3" << endl;

    cout << "Desempate: menor indice" << endl;

    while (running)
    {
        display.showMainMenu();

        int option = readInteger("");

        switch (option)
        {
        case 1:
        {
            int rows =readInteger("Filas (5-50): ");

            int columns =readInteger("Columnas (5-50): ");

            if (world.configure( rows, columns,speciesCount))
            {
                cout << endl;

                cout << "Mundo configurado correctamente." << endl;

                cout << "Se cargo la configuracion oficial."<< endl;
            }
            else
            {
                cout << endl;

                cout << "Configuracion invalida."<< endl;
            }

            break;
        }

        case 2:
        {
            if (!world.isConfigured())
            {
                cout << "Primero configure el mundo."<< endl;

                break;
            }

            cout << endl;

            cout << "===== SIEMBRA MANUAL ====="
                << endl;

            cout << "Ingrese 0 en la fila para terminar."<< endl;

            while (true)
            {
                int row =  readInteger("Fila: ");

                if (row == 0)
                {
                    break;
                }

                int column = readInteger("Columna: ");

                int species =readInteger("Especie (1-3): ");

                if (row < 1 || row > world.getRows() ||column < 1 ||column > world.getColumns())
                {
                    cout << "Coordenadas invalidas." << endl;
                    continue;
                }

                if (species < 1 ||species > world.getSpeciesCount())
                {
                    cout << "Especie invalida." << endl;
                    continue;
                }

                world.seedManual( row - 1,column - 1, species);

                cout << "Organismo colocado."<< endl;
            }

            break;
        }

        case 3:
        {
            if (!world.isConfigured())
            {
                cout << "Primero configure el mundo."<< endl;
                break;
            }

            double density = readDouble( "Densidad (0.0 - 1.0): ");

            if (density < 0.0 || density > 1.0)
            {
                cout << "La densidad debe estar entre 0 y 1." << endl;
            }
            else
            {
                world.seedRandom(density);

                cout << "Mundo sembrado aleatoriamente."<< endl;
            }

            break;
        }

        case 4:
        {
            if (!world.isConfigured())
            {
                cout << "Primero configure el mundo."<< endl;
                break;
            }

            cout << endl;

            cout << "========== GLIDER ==========" << endl;

            cout << ". W ." << endl;
            cout << ". . S" << endl;
            cout << "G G G" << endl;

            cout << endl;

            int row = readInteger("Fila inicial: ");

            int column =readInteger( "Columna inicial: ");

            if (row < 1 || row + 2 > world.getRows() || column < 1 || column + 2 > world.getColumns())
            {
                cout << "El patron no cabe en esa posicion." << endl;
            }
            else
            {
                world.seedPattern(row - 1, column - 1);

                cout << "Patron colocado." << endl;
            }

            break;
        }

        case 5:
        {
            if (!world.isConfigured())
            {
                cout << "Primero configure el mundo." << endl;

                break;
            }

            world.display();

            break;
        }

        case 6:
        {
            if (!world.isConfigured())
            {
                cout << "Primero configure el mundo." << endl;
                break;
            }

            world.evolve();

            world.display();

            break;
        }

        case 7:
        {
            if (!world.isConfigured())
            {
                cout << "Primero configure el mundo." << endl;
                break;
            }

            int generations =
                readInteger("Cantidad de generaciones: ");

            if (generations <= 0)
            {
                cout << "Debe ser mayor que cero." << endl;
                break;
            }

            for (int i = 0;i < generations;i++)
            {
                world.evolve();

                world.display();
            }
            break;
        }

        case 8:
        {
            if (!world.isConfigured())
            {
                cout << "Primero configure el mundo." << endl;
                break;
            }

            world.showStatistics();

            break;
        }

        case 9:
        {
            if (!world.isConfigured())
            {
                cout << "Primero configure el mundo."<< endl;
                break;
            }

            cout << endl;

            cout << "======= CONFIGURAR REGLAS =======" << endl;

            int birthCount =
                readInteger("Cantidad de valores de nacimiento: ");

            if (birthCount <= 0 || birthCount > 8)
            {
                cout << "Cantidad invalida."<< endl;
                break;
            }

            int* birthRules = new int[birthCount];

            for (int i = 0; i < birthCount;i++)
            {
                birthRules[i] = readInteger("Valor de nacimiento: ");
            }

            world.setBirthRules(birthRules, birthCount);

            delete[] birthRules;

            for (int species = 1;species <= world.getSpeciesCount();species++)
            {
                cout << endl;

                cout << "Reglas de supervivencia E"<< species << endl;

                int survivalCount = readInteger("Cantidad de valores: ");

                if (survivalCount <= 0 ||survivalCount > 8)
                {
                    cout << "Cantidad invalida." << endl;
                    break;
                }

                int* survivalRules =new int[survivalCount];

                for (int i = 0;i < survivalCount;i++)
                {
                    survivalRules[i] =readInteger("Valor de supervivencia: ");
                }

                world.setSpeciesSurvivalRules(species,survivalRules, survivalCount);

                delete[] survivalRules;
            }

            cout << endl;

            cout << "Reglas actualizadas correctamente."<< endl;

            break;
        }

        case 10:
        {
            if (!world.isConfigured())
            {
                cout << "Primero configure el mundo." << endl;
                break;
            }

            world.showRules();

            break;
        }

        case 0:
        {
            cout << endl;

            cout << "Saliendo del simulador..."<< endl;

            running = false;

            break;
        }

        default:
        {
            cout << "Opcion invalida." << endl;

            break;
        }
        }
    }

    return 0;
}