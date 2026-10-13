#include "species.h"
SpeciesBase::SpeciesBase()
{
    for (int i = 0; i <= 8; i++)
    {
        birth[i] = i == 3 || i == 6;
        survival[i] = i == 2 || i == 3;
    }
}
bool SpeciesBase::canBeBorn(int neighbors) const { return getRule(true, neighbors); }
bool SpeciesBase::survives(int neighbors) const { return getRule(false, neighbors); }
bool SpeciesBase::getRule(bool isBirth, int neighbors) const
{
    if (neighbors < 0 || neighbors > 8) return false;
    return isBirth ? birth[neighbors] : survival[neighbors];
}
bool SpeciesBase::toggleRule(bool isBirth, int neighbors)
{
    if (neighbors < 0 || neighbors > 8) return false;
    if (isBirth) birth[neighbors] = !birth[neighbors];
    else survival[neighbors] = !survival[neighbors];
    return true;
}
SpeciesBase* Worker::clone() const { return new Worker(*this); }
const char* Worker::getName() const { return "Trabajador"; }
char Worker::getSymbol() const { return 'W'; }
unsigned int Worker::getColor() const { return 0x48D8B0; }
int Worker::getShape() const { return 0; }
SpeciesBase* Scout::clone() const { return new Scout(*this); }
const char* Scout::getName() const { return "Explorador"; }
char Scout::getSymbol() const { return 'S'; }
unsigned int Scout::getColor() const { return 0x62B6FF; }
int Scout::getShape() const { return 1; }
SpeciesBase* Guardian::clone() const { return new Guardian(*this); }
const char* Guardian::getName() const { return "Guardian"; }
char Guardian::getSymbol() const { return 'G'; }
unsigned int Guardian::getColor() const { return 0xFFCA68; }
int Guardian::getShape() const { return 2; }
SpeciesBase* Queen::clone() const { return new Queen(*this); }
const char* Queen::getName() const { return "Reina"; }
char Queen::getSymbol() const { return 'Q'; }
unsigned int Queen::getColor() const { return 0xD69AFF; }
int Queen::getShape() const { return 3; }
