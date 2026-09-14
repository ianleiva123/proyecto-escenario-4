#pragma once
#include <string>

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