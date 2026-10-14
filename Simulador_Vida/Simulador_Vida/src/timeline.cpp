#include "timeline.h"
#include <utility>
Timeline::HistoryNode::HistoryNode(const World& world) : state(world), previous(nullptr), next(nullptr) {}
Timeline::Timeline(const World& world) { first = current = new HistoryNode(world); }
void Timeline::release()
{
    while (first != nullptr) { HistoryNode* next = first->next; delete first; first = next; }
    current = nullptr;
}
void Timeline::copyFrom(const Timeline& other)
{
    first = current = nullptr;
    HistoryNode* tail = nullptr;
    for (HistoryNode* source = other.first; source != nullptr; source = source->next)
    {
        HistoryNode* node = new HistoryNode(source->state);
        node->previous = tail;
        if (tail != nullptr) tail->next = node; else first = node;
        if (source == other.current) current = node;
        tail = node;
    }
}
Timeline::Timeline(const Timeline& other) { copyFrom(other); }
Timeline::~Timeline() { release(); }
Timeline& Timeline::operator=(const Timeline& other)
{
    if (this != &other) { Timeline copy(other); std::swap(first, copy.first); std::swap(current, copy.current); }
    return *this;
}
const World& Timeline::getWorld() const { return current->state; }
World& Timeline::editWorld() { return current->state; }
void Timeline::discardFuture()
{
    HistoryNode* node = current->next;
    current->next = nullptr;
    while (node != nullptr) { HistoryNode* next = node->next; delete node; node = next; }
}
bool Timeline::backward()
{
    if (!hasPrevious()) return false;
    current = current->previous; return true;
}
void Timeline::forward()
{
    if (current->next != nullptr) { current = current->next; return; }
    World next(current->state);
    next.evolve();
    HistoryNode* node = new HistoryNode(next);
    node->previous = current; current->next = node; current = node;
}
bool Timeline::hasPrevious() const { return current->previous != nullptr; }
bool Timeline::hasNext() const { return current->next != nullptr; }
int Timeline::getHistoryCount() const
{
    int count = 0;
    for (HistoryNode* node = first; node != nullptr; node = node->next) count++;
    return count;
}
void Timeline::renameAll(const string& name)
{
    for (HistoryNode* node = first; node != nullptr; node = node->next) node->state.setName(name);
}
