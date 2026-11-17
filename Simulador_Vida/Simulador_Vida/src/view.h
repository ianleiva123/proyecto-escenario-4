#pragma once
#include "laboratory.h"
class View
{
  public:
    virtual ~View() = default;
    virtual void run(Laboratory &laboratory) = 0;
};
