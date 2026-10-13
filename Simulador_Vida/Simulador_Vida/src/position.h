#pragma once

class Position
{
private:
    int row;
    int column;

public:
    Position();
    Position(int newRow, int newColumn);

    int getRow() const;
    int getColumn() const;
};
