#include "rule_list.h"

RuleList::RuleList()
{
    values = nullptr;
    count = 0;
}

RuleList::RuleList(const int* newValues, int newCount)
{
    values = nullptr;
    count = 0;

    if (newCount > 0)
    {
        count = newCount;
        values = new int[count];

        for (int i = 0; i < count; i++)
        {
            values[i] = newValues[i];
        }
    }
}

RuleList::RuleList(const RuleList& other)
{
    values = nullptr;
    count = other.count;

    if (count > 0)
    {
        values = new int[count];

        for (int i = 0; i < count; i++)
        {
            values[i] = other.values[i];
        }
    }
}

RuleList::~RuleList()
{
    delete[] values;
}

RuleList& RuleList::operator=(const RuleList& other)
{
    if (this != &other)
    {
        delete[] values;

        values = nullptr;
        count = other.count;

        if (count > 0)
        {
            values = new int[count];

            for (int i = 0; i < count; i++)
            {
                values[i] = other.values[i];
            }
        }
    }

    return *this;
}

int RuleList::getCount() const
{
    return count;
}

int RuleList::getValue(int position) const
{
    if (position >= 0 && position < count)
    {
        return values[position];
    }

    return -1;
}

bool RuleList::contains(int value) const
{
    for (int i = 0; i < count; i++)
    {
        if (values[i] == value)
        {
            return true;
        }
    }

    return false;
}

bool RuleList::isValid() const
{
    if (count <= 0)
    {
        return false;
    }

    for (int i = 0; i < count; i++)
    {
        if (values[i] < MIN_NEIGHBORS || values[i] > MAX_NEIGHBORS)
        {
            return false;
        }
    }

    return true;
}
