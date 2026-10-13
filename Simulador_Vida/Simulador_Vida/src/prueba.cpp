#include "world.h"
#include <iostream>
#include <cstdlib>
using namespace std;
void check(bool valid, const char* message)
{
    if (!valid) { cerr << message << endl; exit(1); }
}
int main()
{
    World world("Prueba", 5, 5, 4);
    check(world.getSpecies(4).getSymbol() == 'Q', "Reina definida");
    check(world.seedPattern(0, 0), "Patron valido");
    check(world.getCell(0, 1) == 1 && world.getCell(1, 2) == 2 && world.getCell(2, 1) == 3, "Patron exacto");
    World copy(world);
    copy.setCell(0, 1, 4);
    check(world.getCell(0, 1) == 1, "Copia independiente");
    world.clear();
    world.setCell(2, 1, 1); world.setCell(2, 2, 1); world.setCell(2, 3, 1);
    world.evolve();
    check(world.getPopulation(0) == 3 && world.getCell(1, 2) == 1 && world.getCell(3, 2) == 1, "Evolucion simultanea");
    cout << "Modelo correcto" << endl;
}
