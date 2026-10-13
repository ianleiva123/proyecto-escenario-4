#include "world_size.h"

WorldSize::WorldSize()
{
    rows = 0;
    columns = 0;
}

WorldSize::WorldSize(int newRows, int newColumns)
{
    rows = newRows;
    columns = newColumns;
}

int WorldSize::getRows() const
{
    return rows;
}

int WorldSize::getColumns() const
{
    return columns;
}
