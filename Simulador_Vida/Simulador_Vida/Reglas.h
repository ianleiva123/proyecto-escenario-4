#pragma once
#include <string>

// [REVISION] BP-A2: "using namespace std;" en una cabecera contamina todo archivo que la incluya; use std::string aqui.
using namespace std;

class SpeciesRules
{
private:
    int index;
    string name;

    int* survivalRules;
    int survivalCount;

public:
    SpeciesRules();
    SpeciesRules(int index, string name, int* rules, int count);
    SpeciesRules(const SpeciesRules& other);
    ~SpeciesRules();

    SpeciesRules& operator=(const SpeciesRules& other);

    int getIndex() const;
    string getName() const;
    int getSurvivalCount() const;
    int getSurvivalRule(int position) const;

    bool survives(int neighbors) const;

    void setSurvivalRules(int* rules, int count);
};