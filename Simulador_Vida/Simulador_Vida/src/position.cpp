#include "position.h"

Position::Position()
{
    row = 0;
    column = 0;
}

Position::Position(int newRow, int newColumn)
{
    row = newRow;
    column = newColumn;
}

int Position::getRow() const
{
    return row;
}

int Position::getColumn() const
{
    return column;
}
