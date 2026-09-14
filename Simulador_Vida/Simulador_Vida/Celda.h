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
