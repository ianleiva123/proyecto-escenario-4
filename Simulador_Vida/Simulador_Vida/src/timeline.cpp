#include "timeline.h"
#include <algorithm>
#include <utility>
Timeline::HistoryNode::HistoryNode(const World &world) : state(world), previous(nullptr), next(nullptr)
{
}
Timeline::Timeline(const World &world, int limit)
    : first(nullptr), current(nullptr), nodeCount(0),
      historyLimit(std::max(2, std::min(limit, MAX_HISTORY_LIMIT))), discardedPast(0)
{
    first = new HistoryNode(world);
    current = first;
    nodeCount = 1;
}
void Timeline::release()
{
    while (first != nullptr)
    {
        HistoryNode *nextNode = first->next;
        delete first;
        first = nextNode;
    }
    current = nullptr;
    nodeCount = 0;
}
void Timeline::copyFrom(const Timeline &other)
{
    HistoryNode *tail = nullptr;
    try
    {
        for (HistoryNode *source = other.first; source != nullptr; source = source->next)
        {
            HistoryNode *node = new HistoryNode(source->state);
            node->previous = tail;
            if (tail != nullptr)
            {
                tail->next = node;
            }
            else
            {
                first = node;
            }
            if (source == other.current)
            {
                current = node;
            }
            tail = node;
            nodeCount++;
        }
    }
    catch (...)
    {
        release();
        throw;
    }
}
Timeline::Timeline(const Timeline &other)
    : first(nullptr), current(nullptr), nodeCount(0), historyLimit(other.historyLimit),
      discardedPast(other.discardedPast)
{
    copyFrom(other);
}
Timeline::~Timeline()
{
    release();
}
Timeline &Timeline::operator=(const Timeline &other)
{
    if (this != &other)
    {
        Timeline copy(other);
        std::swap(first, copy.first);
        std::swap(current, copy.current);
        std::swap(nodeCount, copy.nodeCount);
        std::swap(historyLimit, copy.historyLimit);
        std::swap(discardedPast, copy.discardedPast);
    }
    return *this;
}
const World &Timeline::getWorld() const
{
    return current->state;
}
World &Timeline::editWorld()
{
    return current->state;
}
void Timeline::discardFuture()
{
    HistoryNode *node = current->next;
    current->next = nullptr;
    while (node != nullptr)
    {
        HistoryNode *nextNode = node->next;
        delete node;
        node = nextNode;
        nodeCount--;
    }
}
bool Timeline::backward()
{
    if (!hasPrevious())
    {
        return false;
    }
    current = current->previous;
    return true;
}
void Timeline::appendSnapshot(const World &world)
{
    HistoryNode *node = new HistoryNode(world);
    node->previous = current;
    current->next = node;
    current = node;
    nodeCount++;
}
bool Timeline::forward()
{
    if (current->next != nullptr)
    {
        current = current->next;
        return true;
    }
    World nextWorld(current->state);
    if (!nextWorld.evolve())
    {
        return false;
    }
    appendSnapshot(nextWorld);
    if (nodeCount > historyLimit)
    {
        HistoryNode *oldFirst = first;
        first = first->next;
        first->previous = nullptr;
        delete oldFirst;
        nodeCount--;
        discardedPast++;
    }
    return true;
}
bool Timeline::hasPrevious() const
{
    return current->previous != nullptr;
}
bool Timeline::hasNext() const
{
    return current->next != nullptr;
}
int Timeline::getHistoryCount() const
{
    return nodeCount;
}
int Timeline::getHistoryLimit() const
{
    return historyLimit;
}
long long Timeline::getDiscardedPast() const
{
    return discardedPast;
}
int Timeline::getCursorPosition() const
{
    int position = 0;
    for (HistoryNode *node = first; node != current; node = node->next)
    {
        position++;
    }
    return position;
}
void Timeline::renameAll(const std::string &name)
{
    for (HistoryNode *node = first; node != nullptr; node = node->next)
    {
        node->state.setName(name);
    }
}
