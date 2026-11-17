#include "sfml_view.h"
#include <cstdlib>
#include <ctime>
int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    Configuration configuration;
    std::string startupMessage;
    startupMessage = "Parámetros seguros internos.";
    Laboratory laboratory(configuration);
    laboratory.setSpeed(configuration.getDefaultSpeed());
    laboratory.setDensity(configuration.getDefaultDensity());
    laboratory.create("Enjambre", configuration.getDefaultSize(), configuration.getDefaultSpecies());
    SfmlView graphicalView(configuration, startupMessage);
    View *view = &graphicalView;
    view->run(laboratory);
    return 0;
}
