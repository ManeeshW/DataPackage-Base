#include "fdcl/system.hpp"

#include <termios.h>
#include <unistd.h>

int getch(void);


void fdcl::thread_key(void)
{
    std::cout << "KEY: thread started" << std::endl;

    char c;
    while (rover.on)
    {
        c = getch();

        if (c == 'q') 
        {
            std::cout << "\nKEY: program end signal received\n" << std::endl;
            rover.turn_off();
        }

        usleep(100000); // 10 Hz
    }

    std::cout << "KEY: thread closed" << std::endl;
}


// This function converts the user input in the terminal to a char.
int getch(void)
{
    struct termios oldattr, newattr;
    int ch;

    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

    return ch;
}