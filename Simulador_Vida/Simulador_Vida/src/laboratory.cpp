#include "laboratory.h"
#include "configuration.h"
#include <utility>
Laboratory::WorldNode::WorldNode(int newId, const World &world, int limit)
    : id(newId), history(world, limit), next(nullptr)
{
}
Laboratory::WorldNode::WorldNode(int newId, const Timeline &timeline)
    : id(newId), history(timeline), next(nullptr)
{
}
Laboratory::Laboratory(const Configuration &config)
    : configuration(config), first(nullptr), active(nullptr), count(0), nextId(1),
      speed(config.getDefaultSpeed()), density(config.getDefaultDensity()), selectedSpecies(1), playing(false)
{
}
Laboratory::~Laboratory()
{
    clear();
}
void Laboratory::clear()
{
    while (first != nullptr)
    {
        WorldNode *nextNode = first->next;
        delete first;
        first = nextNode;
    }
    active = nullptr;
    count = 0;
    nextId = 1;
    selectedSpecies = 1;
    playing = false;
}
void Laboratory::swap(Laboratory &other) noexcept
{
    std::swap(configuration, other.configuration);
    std::swap(first, other.first);
    std::swap(active, other.active);
    std::swap(count, other.count);
    std::swap(nextId, other.nextId);
    std::swap(speed, other.speed);
    std::swap(density, other.density);
    std::swap(selectedSpecies, other.selectedSpecies);
    std::swap(playing, other.playing);
}
void Laboratory::append(WorldNode *node)
{
    if (first == nullptr)
    {
        first = node;
    }
    else
    {
        WorldNode *tail = first;
        while (tail->next != nullptr)
        {
            tail = tail->next;
        }
        tail->next = node;
    }
    active = node;
    count++;
    selectSpecies(selectedSpecies);
}
bool Laboratory::create(const std::string &name, WorldSize size, int speciesCount)
{
    if (count >= MAX_WORLDS || nextId >= MAX_GENERATION || !configuration.allows(size, speciesCount) ||
        !World::validConfiguration(name, size, speciesCount))
    {
        return false;
    }
    World world(name, size, speciesCount);
    configuration.applyTo(world);
    WorldNode *node = new WorldNode(nextId, world, configuration.getHistoryLimit());
    append(node);
    nextId++;
    return true;
}
bool Laboratory::duplicateActive()
{
    if (active == nullptr || count >= MAX_WORLDS || nextId >= MAX_GENERATION)
    {
        return false;
    }
    WorldNode *copy = new WorldNode(nextId, active->history);
    try
    {
        std::string name = active->history.getWorld().getName();
        const std::string suffix = " copia";
        if (name.size() > MAX_NAME_LENGTH - suffix.size())
        {
            name.resize(MAX_NAME_LENGTH - suffix.size());
            while (!name.empty() && (static_cast<unsigned char>(name.back()) & 0xC0) == 0x80)
            {
                name.pop_back();
            }
            if (!name.empty() && static_cast<unsigned char>(name.back()) >= 0xC0)
            {
                name.pop_back();
            }
        }
        copy->history.renameAll(name + suffix);
    }
    catch (...)
    {
        delete copy;
        throw;
    }
    append(copy);
    nextId++;
    return true;
}
bool Laboratory::deleteActive()
{
    if (active == nullptr)
    {
        return false;
    }
    WorldNode *previous = nullptr;
    for (WorldNode *node = first; node != active; node = node->next)
    {
        previous = node;
    }
    WorldNode *victim = active;
    if (previous != nullptr)
    {
        previous->next = victim->next;
    }
    else
    {
        first = victim->next;
    }
    active = victim->next != nullptr ? victim->next : first;
    delete victim;
    count--;
    if (active == nullptr)
    {
        playing = false;
    }
    selectSpecies(selectedSpecies);
    return true;
}
void Laboratory::selectNext()
{
    if (active != nullptr)
    {
        active = active->next != nullptr ? active->next : first;
        selectSpecies(selectedSpecies);
    }
}
void Laboratory::selectPrevious()
{
    if (active == nullptr)
    {
        return;
    }
    WorldNode *previous = nullptr;
    WorldNode *tail = first;
    for (WorldNode *node = first; node != nullptr; node = node->next)
    {
        if (node->next == active)
        {
            previous = node;
        }
        tail = node;
    }
    active = previous != nullptr ? previous : tail;
    selectSpecies(selectedSpecies);
}
int Laboratory::getCount() const
{
    return count;
}
int Laboratory::getActiveId() const
{
    return active == nullptr ? 0 : active->id;
}
const Timeline *Laboratory::getActive() const
{
    return active == nullptr ? nullptr : &active->history;
}
Timeline *Laboratory::getActive()
{
    return active == nullptr ? nullptr : &active->history;
}
int Laboratory::getSpeed() const
{
    return speed;
}
bool Laboratory::setSpeed(int value)
{
    if (value < MIN_SPEED || value > MAX_SPEED || speed == value)
    {
        return false;
    }
    speed = value;
    return true;
}
int Laboratory::getDensity() const
{
    return density;
}
bool Laboratory::setDensity(int value)
{
    if (value < 0 || value > PERCENT_BASE || density == value)
    {
        return false;
    }
    density = value;
    return true;
}
int Laboratory::getSelectedSpecies() const
{
    return selectedSpecies;
}
void Laboratory::selectSpecies(int value)
{
    if (active == nullptr)
    {
        selectedSpecies = 1;
        return;
    }
    int limit = active->history.getWorld().getSpeciesCount();
    selectedSpecies = value >= 1 && value <= limit ? value : 1;
}
bool Laboratory::isPlaying() const
{
    return playing;
}
void Laboratory::setPlaying(bool value)
{
    playing = value && active != nullptr;
}
