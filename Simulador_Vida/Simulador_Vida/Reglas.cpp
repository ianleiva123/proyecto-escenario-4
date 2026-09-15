#include "reglas.h"

SpeciesRules::SpeciesRules()
{
    index = 0;
    name = "";
    survivalRules = nullptr;
    survivalCount = 0;
}

SpeciesRules::SpeciesRules(
    int index,
    string name,
    int* rules,
    int count)
{
    this->index = index;
    this->name = name;
    survivalCount = count;

    if (count > 0)
    {
        survivalRules = new int[count];

        for (int i = 0; i < count; i++)
        {
            survivalRules[i] = rules[i];
        }
    }
    else
    {
        survivalRules = nullptr;
    }
}

SpeciesRules::SpeciesRules(
    const SpeciesRules& other)
{
    index = other.index;
    name = other.name;
    survivalCount = other.survivalCount;

    if (survivalCount > 0)
    {
        survivalRules = new int[survivalCount];

        for (int i = 0; i < survivalCount; i++)
        {
            survivalRules[i] =
                other.survivalRules[i];
        }
    }
    else
    {
        survivalRules = nullptr;
    }
}

SpeciesRules::~SpeciesRules()
{
    delete[] survivalRules;
}

SpeciesRules& SpeciesRules::operator=(
    const SpeciesRules& other)
{
    if (this != &other)
    {
        delete[] survivalRules;

        index = other.index;
        name = other.name;
        survivalCount = other.survivalCount;

        if (survivalCount > 0)
        {
            survivalRules = new int[survivalCount];

            for (int i = 0; i < survivalCount; i++)
            {
                survivalRules[i] =
                    other.survivalRules[i];
            }
        }
        else
        {
            survivalRules = nullptr;
        }
    }

    return *this;
}

int SpeciesRules::getIndex() const
{
    return index;
}

string SpeciesRules::getName() const
{
    return name;
}

int SpeciesRules::getSurvivalCount() const
{
    return survivalCount;
}

int SpeciesRules::getSurvivalRule(
    int position) const
{
    if (position >= 0 &&
        position < survivalCount)
    {
        return survivalRules[position];
    }

    return -1;
}

bool SpeciesRules::survives(
    int neighbors) const
{
    for (int i = 0;
        i < survivalCount;
        i++)
    {
        if (survivalRules[i] == neighbors)
        {
            return true;
        }
    }

    return false;
}

void SpeciesRules::setSurvivalRules(
    int* rules,
    int count)
{
    delete[] survivalRules;

    survivalCount = count;

    if (count > 0)
    {
        survivalRules = new int[count];

        for (int i = 0; i < count; i++)
        {
            survivalRules[i] = rules[i];
        }
    }
    else
    {
        survivalRules = nullptr;
    }
}