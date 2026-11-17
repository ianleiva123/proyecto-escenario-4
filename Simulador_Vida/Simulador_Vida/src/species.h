#pragma once
#include "settings.h"
class SpeciesBase
{
  protected:
    bool birth[RULE_COUNT];
    bool survival[RULE_COUNT];

  public:
    SpeciesBase();
    virtual ~SpeciesBase()
    {
    }
    virtual SpeciesBase *clone() const = 0;
    virtual const char *getName() const = 0;
    virtual char getSymbol() const = 0;
    virtual unsigned int getColor() const = 0;
    virtual Shape getShape() const = 0;
    virtual bool canBeBorn(int neighbors) const;
    virtual bool survives(int neighbors) const;
    bool getRule(bool isBirth, int neighbors) const;
    bool toggleRule(bool isBirth, int neighbors);
    void setRules(const bool newBirth[RULE_COUNT], const bool newSurvival[RULE_COUNT]);
};
class Worker : public SpeciesBase
{
  public:
    SpeciesBase *clone() const override;
    const char *getName() const override;
    char getSymbol() const override;
    unsigned int getColor() const override;
    Shape getShape() const override;
};
class Scout : public SpeciesBase
{
  public:
    SpeciesBase *clone() const override;
    const char *getName() const override;
    char getSymbol() const override;
    unsigned int getColor() const override;
    Shape getShape() const override;
};
class Guardian : public SpeciesBase
{
  public:
    SpeciesBase *clone() const override;
    const char *getName() const override;
    char getSymbol() const override;
    unsigned int getColor() const override;
    Shape getShape() const override;
};
class Queen : public SpeciesBase
{
  public:
    SpeciesBase *clone() const override;
    const char *getName() const override;
    char getSymbol() const override;
    unsigned int getColor() const override;
    Shape getShape() const override;
};
