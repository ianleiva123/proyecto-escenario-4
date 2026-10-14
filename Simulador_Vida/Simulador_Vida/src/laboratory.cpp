#include "laboratory.h"
Laboratory::WorldNode::WorldNode(int newId, const World& world) : id(newId), history(world), next(nullptr) {}
Laboratory::WorldNode::WorldNode(int newId, const Timeline& timeline) : id(newId), history(timeline), next(nullptr) {}
Laboratory::Laboratory() : first(nullptr), active(nullptr), count(0), nextId(1) {}
Laboratory::~Laboratory()
{
    while (first != nullptr) { WorldNode* next = first->next; delete first; first = next; }
}
void Laboratory::append(WorldNode* node)
{
    if (first == nullptr) first = node;
    else { WorldNode* tail = first; while (tail->next != nullptr) tail = tail->next; tail->next = node; }
    active = node; count++;
}
bool Laboratory::create(const string& name, int rows, int columns, int speciesCount)
{
    if (!World::validConfiguration(name, rows, columns, speciesCount)) return false;
    World world(name, rows, columns, speciesCount);
    append(new WorldNode(nextId++, world)); return true;
}
bool Laboratory::duplicateActive()
{
    if (active == nullptr) return false;
    WorldNode* copy = new WorldNode(nextId++, active->history);
    string name = active->history.getWorld().getName();
    if (name.size() > 18) name.resize(18);
    copy->history.renameAll(name + " copia");
    append(copy); return true;
}
bool Laboratory::deleteActive()
{
    if (active == nullptr) return false;
    WorldNode* previous = nullptr;
    for (WorldNode* node = first; node != active; node = node->next) previous = node;
    WorldNode* victim = active;
    if (previous != nullptr) previous->next = victim->next; else first = victim->next;
    active = victim->next != nullptr ? victim->next : first;
    delete victim; count--; return true;
}
void Laboratory::selectNext() { if (active != nullptr) active = active->next != nullptr ? active->next : first; }
void Laboratory::selectPrevious()
{
    if (active == nullptr) return;
    WorldNode* previous = nullptr;
    WorldNode* tail = first;
    for (WorldNode* node = first; node != nullptr; node = node->next)
    { if (node->next == active) previous = node; tail = node; }
    active = previous != nullptr ? previous : tail;
}
bool Laboratory::selectId(int id)
{
    for (WorldNode* node = first; node != nullptr; node = node->next) if (node->id == id) { active = node; return true; }
    return false;
}
int Laboratory::getCount() const { return count; }
int Laboratory::getActiveId() const { return active == nullptr ? 0 : active->id; }
int Laboratory::getIdAt(int index) const
{
    if (index < 0) return 0;
    WorldNode* node = first;
    for (int i = 0; i < index && node != nullptr; i++) node = node->next;
    return node == nullptr ? 0 : node->id;
}
const World* Laboratory::getWorldAt(int index) const
{
    if (index < 0) return nullptr;
    WorldNode* node = first;
    for (int i = 0; i < index && node != nullptr; i++) node = node->next;
    return node == nullptr ? nullptr : &node->history.getWorld();
}
const Timeline* Laboratory::getActive() const { return active == nullptr ? nullptr : &active->history; }
Timeline* Laboratory::getActive() { return active == nullptr ? nullptr : &active->history; }
