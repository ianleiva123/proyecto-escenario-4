// [REVISION] BP-A2/BP-C2: falta la guarda de inclusion (#pragma once) y el archivo Celda.h no coincide con la clase Cell; renombre a cell.h.
// [NOTA] Los nombres de archivo estan en espanol y el curso pide todo el codigo en ingles (BP-N2, PR-X5).
// El contenido si esta en ingles, asi que solo hay que renombrar los archivos y sus #include:
// Celda.h y Celda.cpp pasan a cell.h y cell.cpp, Mundo a world, Reglas a species_rules, Consola a console_display,
// y Simulador_Vida.cpp a main.cpp. La guia ademas pide snake_case para nombres de archivo (BP-A1).
// Sugerencia, no afecta la nota.
class Cell
{
private:
    int species;

public:
    Cell();
    Cell(int species);

    int getSpecies() const;
    void setSpecies(int species);
    bool isAlive() const;
};
