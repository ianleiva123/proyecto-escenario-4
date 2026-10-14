#pragma once
#include "laboratory.h"
class View
{
public:
    virtual ~View() {}
    virtual void run(Laboratory& laboratory) = 0;

};
