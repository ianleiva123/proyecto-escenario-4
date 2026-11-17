#include "configuration.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
namespace
{
std::string trim(const std::string &text)
{
    std::size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
    {
        return "";
    }
    return text.substr(first, text.find_last_not_of(" \t\r\n") - first + 1);
}
int integer(const std::string &text)
{
    int value = 0;
    std::stringstream input(text);
    if (!(input >> value) || !(input >> std::ws).eof())
    {
        throw std::runtime_error("Valor entero inválido.");
    }
    return value;
}
bool flag(const std::string &text)
{
    int value = integer(text);
    if (value != 0 && value != 1)
    {
        throw std::runtime_error("El valor lógico debe ser 0 o 1.");
    }
    return value == 1;
}
void ruleValues(const std::string &text, bool values[RULE_COUNT])
{
    for (int i = 0; i < RULE_COUNT; i++)
    {
        values[i] = false;
    }
    if (text.empty())
    {
        return;
    }
    if (text.back() == ',')
    {
        throw std::runtime_error("Regla incompleta.");
    }
    std::stringstream list(text);
    std::string token;
    while (std::getline(list, token, ','))
    {
        int value = integer(trim(token));
        if (value < 0 || value > MAX_NEIGHBORS || values[value])
        {
            throw std::runtime_error("Regla repetida o fuera de 0–8.");
        }
        values[value] = true;
    }
}
} // namespace
Configuration::Configuration()
    : minRows(5), maxRows(50), minColumns(5), maxColumns(50), defaultSize{20, 25},
      availableSpecies(MAX_SPECIES), defaultSpecies(MIN_SPECIES), defaultSpeed(5), defaultDensity(30),
      historyLimit(DEFAULT_HISTORY_LIMIT), defaultMoore(true), defaultWrapped(false), birth{}, survival{}
{
    for (int id = 0; id < MAX_SPECIES; id++)
    {
        birth[id][3] = true;
        birth[id][6] = true;
        survival[id][2] = true;
        survival[id][3] = true;
    }
}
bool Configuration::valid() const
{
    return minRows >= MIN_SIZE && minRows <= maxRows && maxRows <= MAX_SIZE && minColumns >= MIN_SIZE &&
           minColumns <= maxColumns && maxColumns <= MAX_SIZE && defaultSize.rows >= minRows &&
           defaultSize.rows <= maxRows && defaultSize.columns >= minColumns &&
           defaultSize.columns <= maxColumns && availableSpecies >= MIN_SPECIES &&
           availableSpecies <= MAX_SPECIES && defaultSpecies >= MIN_SPECIES &&
           defaultSpecies <= availableSpecies && defaultSpeed >= MIN_SPEED && defaultSpeed <= MAX_SPEED &&
           defaultDensity >= 0 && defaultDensity <= PERCENT_BASE && historyLimit >= 2 &&
           historyLimit <= MAX_HISTORY_LIMIT;
}
bool Configuration::load(const std::string &path, std::string &message)
{
    *this = Configuration();
    std::ifstream file(path);
    if (!file)
    {
        message = "No se pudo leer config.ini; se usan valores seguros.";
        return false;
    }
    Configuration candidate;
    std::string section;
    std::string line;
    std::string seen;
    int lineNumber = 0;
    try
    {
        while (std::getline(file, line))
        {
            lineNumber++;
            if (lineNumber == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            {
                line.erase(0, 3);
            }
            if (line.size() > 4096 || lineNumber > 500)
            {
                throw std::runtime_error("Configuración demasiado extensa.");
            }
            std::size_t comment = line.find_first_of(";#");
            line = trim(line.substr(0, comment));
            if (line.empty())
            {
                continue;
            }
            if (line.front() == '[' && line.back() == ']')
            {
                section = trim(line.substr(1, line.size() - 2));
                if (section != "limits" && section != "defaults" && section != "species" &&
                    section != "species.1" && section != "species.2" && section != "species.3" &&
                    section != "species.4")
                {
                    throw std::runtime_error("Sección desconocida.");
                }
                continue;
            }
            std::size_t equal = line.find('=');
            if (equal == std::string::npos)
            {
                throw std::runtime_error("Falta el signo =.");
            }
            std::string key = section + "." + trim(line.substr(0, equal));
            std::string value = trim(line.substr(equal + 1));
            std::string marker = "\n" + key + "\n";
            if (seen.find(marker) != std::string::npos)
            {
                throw std::runtime_error("Clave repetida.");
            }
            seen += marker;
            if (key == "limits.min_rows")
            {
                candidate.minRows = integer(value);
            }
            else if (key == "limits.max_rows")
            {
                candidate.maxRows = integer(value);
            }
            else if (key == "limits.min_columns")
            {
                candidate.minColumns = integer(value);
            }
            else if (key == "limits.max_columns")
            {
                candidate.maxColumns = integer(value);
            }
            else if (key == "defaults.rows")
            {
                candidate.defaultSize.rows = integer(value);
            }
            else if (key == "defaults.columns")
            {
                candidate.defaultSize.columns = integer(value);
            }
            else if (key == "defaults.species")
            {
                candidate.defaultSpecies = integer(value);
            }
            else if (key == "defaults.speed")
            {
                candidate.defaultSpeed = integer(value);
            }
            else if (key == "defaults.history_limit")
            {
                candidate.historyLimit = integer(value);
            }
            else if (key == "defaults.moore")
            {
                candidate.defaultMoore = flag(value);
            }
            else if (key == "defaults.wrapped")
            {
                candidate.defaultWrapped = flag(value);
            }
            else if (key == "species.available")
            {
                candidate.availableSpecies = integer(value);
            }
            else if (key == "defaults.density")
            {
                double density = 0.0;
                std::stringstream input(value);
                if (!(input >> density) || !(input >> std::ws).eof() || !std::isfinite(density) ||
                    density < 0 || density > 1)
                {
                    throw std::runtime_error("Densidad fuera de 0–1.");
                }
                candidate.defaultDensity = static_cast<int>(std::lround(density * PERCENT_BASE));
            }
            else
            {
                bool matched = false;
                for (int id = 0; id < MAX_SPECIES; id++)
                {
                    std::string prefix = "species." + std::to_string(id + 1);
                    if (key == prefix + ".birth")
                    {
                        ruleValues(value, candidate.birth[id]);
                        matched = true;
                    }
                    else if (key == prefix + ".survival")
                    {
                        ruleValues(value, candidate.survival[id]);
                        matched = true;
                    }
                }
                if (!matched)
                {
                    throw std::runtime_error("Clave desconocida.");
                }
            }
        }
        if (file.bad() || seen.empty() || !candidate.valid())
        {
            throw std::runtime_error("Límites o valores predeterminados incoherentes.");
        }
        *this = candidate;
        message = "Configuración externa cargada.";
        return true;
    }
    catch (const std::exception &error)
    {
        message = "INI inválido, línea " + std::to_string(lineNumber) + ": " + error.what() +
                  " Se usan valores seguros.";
        return false;
    }
}
bool Configuration::allows(WorldSize size, int count) const
{
    return size.rows >= minRows && size.rows <= maxRows && size.columns >= minColumns &&
           size.columns <= maxColumns && count >= MIN_SPECIES && count <= availableSpecies;
}
void Configuration::applyTo(World &world) const
{
    for (int id = 1; id <= world.getSpeciesCount(); id++)
    {
        world.setRules(id, birth[id - 1], survival[id - 1]);
    }
    if (world.isMoore() != defaultMoore)
    {
        world.toggleNeighborhood();
    }
    if (world.isWrapped() != defaultWrapped)
    {
        world.toggleBorder();
    }
}
WorldSize Configuration::getDefaultSize() const
{
    return defaultSize;
}
int Configuration::getDefaultSpecies() const
{
    return defaultSpecies;
}
int Configuration::getAvailableSpecies() const
{
    return availableSpecies;
}
int Configuration::getDefaultSpeed() const
{
    return defaultSpeed;
}
int Configuration::getDefaultDensity() const
{
    return defaultDensity;
}
int Configuration::getHistoryLimit() const
{
    return historyLimit;
}
int Configuration::getMinRows() const
{
    return minRows;
}
int Configuration::getMaxRows() const
{
    return maxRows;
}
int Configuration::getMinColumns() const
{
    return minColumns;
}
int Configuration::getMaxColumns() const
{
    return maxColumns;
}
