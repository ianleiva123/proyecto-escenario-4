#include "laboratory.h"
// Entrada temporal: comprueba gestion de mundos sin pedir datos por consola.
int main()
{
    Laboratory laboratory;
    laboratory.create("Enjambre", 20, 25, 3);
    laboratory.getActive()->editWorld().seedPattern(1, 1);
    laboratory.getActive()->forward();
    laboratory.duplicateActive();
    laboratory.selectPrevious();
    return laboratory.getCount() == 2 ? 0 : 1;
}
