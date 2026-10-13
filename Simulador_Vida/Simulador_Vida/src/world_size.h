#pragma once

class WorldSize
{
private:
    int rows;
    int columns;

public:
    WorldSize();
    WorldSize(int newRows, int newColumns);

    int getRows() const;
    int getColumns() const;
};
