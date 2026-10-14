#pragma once
#include "timeline.h"
class Laboratory
{
private:
    struct WorldNode
    {
        int id;
        Timeline history;
        WorldNode* next;
        WorldNode(int newId, const World& world);
        WorldNode(int newId, const Timeline& timeline);
    };
    WorldNode* first;
    WorldNode* active;
    int count, nextId;
    void append(WorldNode* node);
    Laboratory(const Laboratory&) = delete;
    Laboratory& operator=(const Laboratory&) = delete;
public:
    Laboratory();
    ~Laboratory();
    bool create(const string& name, int rows, int columns, int speciesCount);
    bool duplicateActive();
    bool deleteActive();
    void selectNext();
    void selectPrevious();
    bool selectId(int id);
    int getCount() const;
    int getActiveId() const;
    int getIdAt(int index) const;
    const World* getWorldAt(int index) const;
    const Timeline* getActive() const;
    Timeline* getActive();

};
