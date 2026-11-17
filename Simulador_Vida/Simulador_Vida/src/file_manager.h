#pragma once
#include "laboratory.h"
#include <iosfwd>
class FileManager
{
  private:
    static void writeWorld(std::ostream &output, const World &world);
    static World readWorld(std::istream &input, int &lineNumber, long long &cellsRead);
    static void writeLaboratory(std::ostream &output, const Laboratory &laboratory);
    static void readLaboratory(std::istream &input, Laboratory &laboratory);

  public:
    static bool save(const Laboratory &laboratory, const std::string &path, std::string &message);
    static bool load(Laboratory &laboratory, const std::string &path, std::string &message);
};
