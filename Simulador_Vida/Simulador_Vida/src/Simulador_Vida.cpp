#include "sfml_view.h"
#include <cstdlib>
#include <ctime>
int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    Laboratory laboratory;
    laboratory.create("Enjambre", WorldSize{20, 25}, 3);
    SfmlView graphicalView;
    View *view = &graphicalView;
    view->run(laboratory);
    return 0;
}
