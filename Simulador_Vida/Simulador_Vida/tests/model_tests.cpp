#include "configuration.h"
#include "file_manager.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
int checks = 0;
void check(bool condition, const std::string &detail)
{
    checks++;
    if (!condition)
    {
        throw std::runtime_error(detail);
    }
}
std::string read(const std::string &path)
{
    std::ifstream file(path);
    std::stringstream text;
    text << file.rdbuf();
    return text.str();
}
void write(const std::string &path, const std::string &text)
{
    std::ofstream file(path);
    file << text;
}
std::string snapshot(const Laboratory &laboratory, const std::string &path)
{
    std::string message;
    check(FileManager::save(laboratory, path, message), message);
    return read(path);
}
void equalWorlds(const World &a, const World &b)
{
    check(a.getRows() == b.getRows() && a.getColumns() == b.getColumns(), "Dimensions");
    check(a.getGeneration() == b.getGeneration(), "Generation");
    for (int row = 0; row < a.getRows(); row++)
    {
        for (int column = 0; column < a.getColumns(); column++)
        {
            check(a.getCell(row, column) == b.getCell(row, column), "Cell copy");
        }
    }
}
int main()
{
    try
    {
        Configuration config;
        std::string message;
        std::string ini = "[limits]\nmin_rows=3\nmax_rows=80\nmin_columns=3\nmax_columns=90\n"
                          "[defaults]\nrows=12\ncolumns=14\nspecies=4\nspeed=9\ndensity=0.45\nhistory_limit="
                          "4\nmoore=0\nwrapped=1\n"
                          "[species]\navailable=4\n[species.4]\nbirth=1,4\nsurvival=0,2\n";
        write("test_config.ini", ini);
        check(config.load("test_config.ini", message), "INI valid: " + message);
        check(config.getDefaultSize().rows == 12 && config.getDefaultSize().columns == 14, "INI dimensions");
        check(config.getDefaultDensity() == 45 && config.getDefaultSpeed() == 9 &&
                  config.getHistoryLimit() == 4,
              "INI controls");
        check(config.allows(WorldSize{80, 90}, 4) && !config.allows(WorldSize{81, 90}, 4), "Runtime bounds");
        Laboratory configured(config);
        configured.create("Configurado", config.getDefaultSize(), config.getDefaultSpecies());
        check(!configured.getActive()->getWorld().isMoore() && configured.getActive()->getWorld().isWrapped(),
              "INI neighborhood");
        check(configured.getActive()->getWorld().getSpecies(4).getRule(true, 1) &&
                  !configured.getActive()->getWorld().getSpecies(4).getRule(true, 3),
              "Species defaults");
        write("test_bad.ini", ini + "[defaults]\nspeed=4\n");
        check(!config.load("test_bad.ini", message) && config.getDefaultSpeed() == 5,
              "Duplicate key -> full defaults");
        write("test_bad.ini", "[defaults]\ndensity=1.5\n");
        check(!config.load("test_bad.ini", message), "Invalid density");
        write("test_bad.ini", "[defaults]\nrows=99999999999999999999999\n");
        check(!config.load("test_bad.ini", message), "Integer overflow");
        write("test_bad.ini", "[defaults]\nspeed=5garbage\n");
        check(!config.load("test_bad.ini", message), "Trailing numeric garbage");
        write("test_bad.ini", "[species.1]\nbirth=3,\n");
        check(!config.load("test_bad.ini", message), "Trailing comma");
        write("test_bad.ini", "[species.1]\nbirth=\nsurvival=\n");
        check(config.load("test_bad.ini", message), "Empty rule lists allowed");
        check(!config.load("does_not_exist.ini", message) && config.getDefaultSize().rows == 20,
              "Missing INI defaults");
        World world("Prueba", WorldSize{5, 5}, MAX_SPECIES);
        check(!World::validConfiguration("", WorldSize{5, 5}, 3), "Empty name");
        check(!world.setCell(-1, 0, 1) && !world.setCell(0, 0, 5), "Invalid painting");
        check(world.seedPattern(0, 0), "Pattern fit");
        check(world.getCell(0, 1) == 1 && world.getCell(1, 2) == 2 && world.getCell(2, 1) == 3,
              "Pattern exact");
        World copy(world);
        copy.setCell(0, 1, 4);
        copy.toggleRule(1, true, 3);
        check(world.getCell(0, 1) == 1 && world.getSpecies(1).getRule(true, 3), "Deep copy");
        World assigned;
        assigned = world;
        assigned = assigned;
        equalWorlds(world, assigned);
        world.clear();
        world.setCell(2, 1, 1);
        world.setCell(2, 2, 1);
        world.setCell(2, 3, 1);
        Timeline history(world, 4);
        history.forward();
        check(history.getWorld().getPopulation(0) == 3 && history.getWorld().getCell(1, 2) == 1,
              "Simultaneous evolution");
        history.forward();
        history.backward();
        Timeline independent(history);
        history.editWorld().setCell(0, 0, 4);
        history.discardFuture();
        check(history.getHistoryCount() == 2 && !history.hasNext() && independent.hasNext(),
              "Branch isolation");
        for (int i = 0; i < 10; i++)
        {
            history.forward();
        }
        check(history.getHistoryCount() == 4 && history.getDiscardedPast() > 0, "Bounded history");
        while (history.backward())
        {
        }
        check(!history.backward(), "Cannot move before retained beginning");
        World torus("Borde", WorldSize{5, 5}, 3);
        torus.setCell(4, 4, 1);
        torus.setCell(4, 0, 1);
        torus.setCell(0, 4, 1);
        World closed(torus);
        torus.toggleBorder();
        torus.evolve();
        closed.evolve();
        check(torus.getCell(0, 0) == 1 && closed.getCell(0, 0) == 0, "Toroidal border");
        Laboratory original;
        check(original.create("Árbol", WorldSize{5, 5}, 4), "Create first world");
        original.getActive()->editWorld().seedPattern(1, 1);
        original.getActive()->editWorld().toggleMutation();
        original.getActive()->editWorld().setMutationPercent(100);
        for (int i = 0; i < 5; i++)
        {
            original.getActive()->forward();
        }
        original.getActive()->backward();
        check(original.getActive()->hasNext(), "Future retained");
        original.setPlaying(true);
        original.setSpeed(17);
        original.setDensity(65);
        check(original.getActive()->hasNext() && original.isPlaying(),
              "Speed/density preserve future and playback");
        original.getActive()->backward();
        check(original.getSpeed() == 17 && original.getDensity() == 65, "Controls independent of timeline");
        original.selectSpecies(4);
        check(original.duplicateActive(), "Duplicate complete history");
        original.getActive()->editWorld().setCell(0, 0, 4);
        original.getActive()->discardFuture();
        original.create("Otro mundo", WorldSize{7, 8}, 3);
        original.getActive()->editWorld().seedRandom(45);
        original.selectPrevious();
        original.selectSpecies(4);
        original.setPlaying(true);
        std::string before = snapshot(original, "test_before.txt");
        Laboratory loaded;
        check(FileManager::load(loaded, "test_before.txt", message), message);
        check(before == snapshot(loaded, "test_after.txt"),
              "Exact round trip: every world, rule, cursor, setting, RNG");
        loaded.getActive()->forward();
        original.getActive()->forward();
        check(snapshot(original, "test_before.txt") == snapshot(loaded, "test_after.txt"),
              "Same random continuation");
        original.selectPrevious();
        loaded.selectPrevious();
        check(original.getActive()->hasNext() && loaded.getActive()->hasNext(), "Original future persisted");
        original.getActive()->forward();
        loaded.getActive()->forward();
        check(snapshot(original, "test_before.txt") == snapshot(loaded, "test_after.txt"),
              "Stored future restored exactly");
        original.selectSpecies(2);
        original.setPlaying(true);
        std::string safe = snapshot(original, "test_safe.txt");
        write("test_corrupt.txt", "ENJAMBRE_LAB 99\n");
        check(!FileManager::load(original, "test_corrupt.txt", message), "Reject unsupported format");
        check(safe == snapshot(original, "test_after.txt"), "Failed load preserves original");
        check(!FileManager::load(original, "missing_save.txt", message), "Missing save");
        check(safe == snapshot(original, "test_after.txt"), "Missing save preserves original");
        for (std::size_t length = 0; length < safe.size(); length += 13)
        {
            write("test_corrupt.txt", safe.substr(0, length));
            check(!FileManager::load(original, "test_corrupt.txt", message), "Reject truncated save");
            check(safe == snapshot(original, "test_after.txt"), "Truncation is transactional");
        }
        write("test_corrupt.txt", safe + "GARBAGE\n");
        check(!FileManager::load(original, "test_corrupt.txt", message), "Reject extra records");
        std::string invalid = safe;
        std::size_t row = invalid.find("ROW ");
        invalid.replace(row, 5, "ROW 9");
        write("test_corrupt.txt", invalid);
        check(!FileManager::load(original, "test_corrupt.txt", message),
              "Reject species outside world range");
        check(safe == snapshot(original, "test_after.txt"), "Invalid cell preserves open lab");
        check(!FileManager::save(original, "missing_folder/lab.txt", message), "Unwritable target");
        check(safe == read("test_safe.txt"), "Previous file intact");
        Laboratory empty;
        check(FileManager::load(empty, "test_safe.txt", message), "Populate destination");
        while (empty.deleteActive())
        {
        }
        check(empty.getCount() == 0 && !empty.isPlaying() && empty.getSelectedSpecies() == 1,
              "Delete final active state");
        std::string emptySave = snapshot(empty, "test_empty.txt");
        check(FileManager::load(loaded, "test_empty.txt", message), "Load empty laboratory");
        check(emptySave == snapshot(loaded, "test_after.txt"), "Empty lab round trip");
        check(loaded.create("Después", WorldSize{5, 5}, 3), "Continue after empty load");
        Configuration capped;
        write("test_config.ini", "[defaults]\nhistory_limit=3\n");
        check(capped.load("test_config.ini", message), "Cap config");
        Laboratory limited(capped);
        limited.create("Límite", WorldSize{5, 5}, 3);
        for (int i = 0; i < 15; i++)
        {
            limited.getActive()->forward();
        }
        limited.getActive()->backward();
        std::string capSave = snapshot(limited, "test_cap.txt");
        check(FileManager::load(loaded, "test_cap.txt", message), message);
        check(capSave == snapshot(loaded, "test_after.txt"), "Pruned history and cursor round trip");
        std::cout << "Correcto: " << checks << " comprobaciones." << std::endl;
        const char *paths[] = {"test_config.ini", "test_bad.ini",     "test_before.txt", "test_after.txt",
                               "test_safe.txt",   "test_corrupt.txt", "test_empty.txt",  "test_cap.txt"};
        for (const char *path : paths)
        {
            std::remove(path);
        }
        return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << "FALLO: " << error.what() << std::endl;
        return 1;
    }
}
