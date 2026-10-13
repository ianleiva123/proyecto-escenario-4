#pragma once
#include <string>

#include "rule_list.h"

class SpeciesRules
{
private:
    std::string name;
    RuleList survivalRuleList;

public:
    SpeciesRules();
    SpeciesRules(const std::string& newName, const RuleList& newRules);

    std::string getName() const;
    const RuleList& getSurvivalRules() const;

    bool survives(int neighbors) const;

    bool setSurvivalRules(const RuleList& newRules);
};
