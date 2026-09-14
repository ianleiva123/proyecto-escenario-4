#include "Consola.h"

#include <iostream>

using namespace std;

void ConsoleDisplay::showHeader() const
{
    cout << endl;

    cout << "============================================"
        << endl;

    cout << "        SIMULADOR DE MUNDOS VIVOS"
        << endl;

    cout << "                 ENJAMBRE"
        << endl;

    cout << "============================================"
        << endl;
}

void ConsoleDisplay::showMainMenu() const
{
    cout << endl;

    cout << "============== MENU PRINCIPAL =============="
        << endl;

    cout << "1. Configurar mundo" << endl;
    cout << "2. Sembrar manualmente" << endl;
    cout << "3. Sembrar aleatoriamente" << endl;
    cout << "4. Colocar patron" << endl;
    cout << "5. Mostrar mundo" << endl;
    cout << "6. Avanzar una generacion" << endl;
    cout << "7. Avanzar varias generaciones" << endl;
    cout << "8. Ver estadisticas" << endl;
    cout << "9. Configurar reglas" << endl;
    cout << "10. Ver reglas" << endl;
    cout << "0. Salir" << endl;

    cout << "============================================"
        << endl;

    cout << "Seleccione una opcion: ";
}