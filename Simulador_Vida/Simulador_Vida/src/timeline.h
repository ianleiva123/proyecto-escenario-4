#pragma once
#include "world.h"
class Timeline
{
private:
    struct HistoryNode
    { 

        World state;
        HistoryNode* previous;
        HistoryNode* next;
        explicit HistoryNode(const World& world);
    };
    HistoryNode* first;
    HistoryNode* current;
    void release();
    void copyFrom(const Timeline& other);
public:
    explicit Timeline(const World& world);
    Timeline(const Timeline& other);
    ~Timeline();
    Timeline& operator=(const Timeline& other);
    const World& getWorld() const;
    World& editWorld();
    void discardFuture();
    bool backward();
    void forward();
    bool hasPrevious() const;
    bool hasNext() const;
    int getHistoryCount() const;
    void renameAll(const string& name);
};
