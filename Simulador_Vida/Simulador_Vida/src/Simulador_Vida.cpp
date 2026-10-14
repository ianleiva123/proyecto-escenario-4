#include <cstdlib>
#include <ctime>
#include "sfml_view.h"
int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    Laboratory laboratory;
    laboratory.create("Enjambre", 20, 25, 3);
    SfmlView graphicalView;
    View* view = &graphicalView; 
    view->run(laboratory);
    return 0;
}
