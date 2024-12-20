#include "fdcl/system.hpp"
#include "fdcl/gui.hpp"


void fdcl::thread_gui(void) {
    fdcl::Gui gui;

    gui.run();
}