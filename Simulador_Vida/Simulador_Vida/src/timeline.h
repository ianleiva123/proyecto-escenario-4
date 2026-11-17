#pragma once
#include "world.h"
class FileManager;
class Timeline
{
    friend class FileManager;

  private:
    struct HistoryNode
    {
        World state;
        HistoryNode *previous;
        HistoryNode *next;
        explicit HistoryNode(const World &world);
    };
    HistoryNode *first;
    HistoryNode *current;
    int nodeCount;
    int historyLimit;
    long long discardedPast;
    void release();
    void copyFrom(const Timeline &other);
    void appendSnapshot(const World &world);

  public:
    explicit Timeline(const World &world, int limit = DEFAULT_HISTORY_LIMIT);
    Timeline(const Timeline &other);
    ~Timeline();
    Timeline &operator=(const Timeline &other);
    const World &getWorld() const;
    World &editWorld();
    void discardFuture();
    bool backward();
    bool forward();
    bool hasPrevious() const;
    bool hasNext() const;
    int getHistoryCount() const;
    int getHistoryLimit() const;
    long long getDiscardedPast() const;
    int getCursorPosition() const;
    void renameAll(const std::string &name);
};
