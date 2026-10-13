#include "species_rules.h"

SpeciesRules::SpeciesRules()
{
    name = "";
}

SpeciesRules::SpeciesRules(const std::string& newName, const RuleList& newRules)
{
    name = newName;
    survivalRuleList = newRules;
}

std::string SpeciesRules::getName() const
{
    return name;
}

const RuleList& SpeciesRules::getSurvivalRules() const
{
    return survivalRuleList;
}

bool SpeciesRules::survives(int neighbors) const
{
    return survivalRuleList.contains(neighbors);
}

bool SpeciesRules::setSurvivalRules(const RuleList& newRules)
{
    if (!newRules.isValid())
    {
        return false;
    }

    survivalRuleList = newRules;

    return true;
}
