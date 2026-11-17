#pragma once
#include "configuration.h"
#include "timeline.h"
class FileManager;
class Laboratory
{
    friend class FileManager;

  private:
    struct WorldNode
    {
        int id;
        Timeline history;
        WorldNode *next;
        WorldNode(int id, const World &world, int historyLimit);
        WorldNode(int id, const Timeline &history);
    };
    Configuration configuration;
    WorldNode *first;
    WorldNode *active;
    int count;
    int nextId;
    int speed;
    int density;
    int selectedSpecies;
    bool playing;
    void append(WorldNode *node);
    Laboratory(const Laboratory &) = delete;
    Laboratory &operator=(const Laboratory &) = delete;

  public:
    explicit Laboratory(const Configuration &configuration = Configuration());
    ~Laboratory();
    void clear();
    void swap(Laboratory &other) noexcept;
    bool create(const std::string &name, WorldSize size, int speciesCount);
    bool duplicateActive();
    bool deleteActive();
    void selectNext();
    void selectPrevious();
    int getCount() const;
    int getActiveId() const;
    const Timeline *getActive() const;
    Timeline *getActive();
    int getSpeed() const;
    bool setSpeed(int value);
    int getDensity() const;
    bool setDensity(int value);
    int getSelectedSpecies() const;
    void selectSpecies(int value);
    bool isPlaying() const;
    void setPlaying(bool value);
};
