#pragma once
class SpeciesBase
{
protected:
    bool birth[9];
    bool survival[9];
public:
    SpeciesBase();
    virtual ~SpeciesBase() {}
    virtual SpeciesBase* clone() const = 0;
    virtual const char* getName() const = 0;
    virtual char getSymbol() const = 0;
    virtual unsigned int getColor() const = 0;
    virtual int getShape() const = 0;
    virtual bool canBeBorn(int neighbors) const;
    virtual bool survives(int neighbors) const;
    bool getRule(bool isBirth, int neighbors) const;
    bool toggleRule(bool isBirth, int neighbors);
};
class Worker : public SpeciesBase
{
public:
    SpeciesBase* clone() const override;
    const char* getName() const override;
    char getSymbol() const override;
    unsigned int getColor() const override;
    int getShape() const override;
};
class Scout : public SpeciesBase
{
public:
    SpeciesBase* clone() const override;
    const char* getName() const override;
    char getSymbol() const override;
    unsigned int getColor() const override;
    int getShape() const override;
};
class Guardian : public SpeciesBase
{
public:
    SpeciesBase* clone() const override;
    const char* getName() const override;
    char getSymbol() const override;
    unsigned int getColor() const override;
    int getShape() const override;

};
class Queen : public SpeciesBase
{
public:
    SpeciesBase* clone() const override;
    const char* getName() const override;
    char getSymbol() const override;
    unsigned int getColor() const override;
    int getShape() const override;
};
