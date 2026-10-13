#include "console_display.h"

#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

using namespace std;

void ConsoleDisplay::setupEncoding() const
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void ConsoleDisplay::showHeader() const
{
    cout << endl;

    cout << "============================================" << endl;
    cout << "        SIMULADOR DE MUNDOS VIVOS" << endl;
    cout << "                 ENJAMBRE" << endl;
    cout << "============================================" << endl;
}

void ConsoleDisplay::showMainMenu() const
{
    cout << endl;

    cout << "============== MENÚ PRINCIPAL ==============" << endl;

    cout << "1. Configurar mundo" << endl;
    cout << "2. Sembrar manualmente" << endl;
    cout << "3. Sembrar aleatoriamente" << endl;
    cout << "4. Colocar patrón" << endl;
    cout << "5. Mostrar mundo" << endl;
    cout << "6. Avanzar una generación" << endl;
    cout << "7. Avanzar varias generaciones" << endl;
    cout << "8. Ver estadísticas" << endl;
    cout << "9. Configurar reglas" << endl;
    cout << "10. Ver reglas" << endl;
    cout << "0. Salir" << endl;

    cout << "============================================" << endl;

    cout << "Seleccione una opción: ";
}
