#pragma once

const int MIN_NEIGHBORS = 0;
const int MAX_NEIGHBORS = 8;

class RuleList
{
private:
    int* values;
    int count;

public:
    RuleList();
    RuleList(const int* newValues, int newCount);
    RuleList(const RuleList& other);
    ~RuleList();

    RuleList& operator=(const RuleList& other);

    int getCount() const;
    int getValue(int position) const;

    bool contains(int value) const;
    bool isValid() const;
};
