#include "fdcl/clock.hpp"

#include <iostream>
#include <unistd.h>


int main (void)
{
    fdcl::Clock Clock("Main Timer");
    unsigned long long t_ns = 0;
    double t_millis = 0.0;
    double t_seconds = 0.0;

    Clock.get_date_time();
    std::cout << "\nToday is " << Clock.get_date() 
        << " and the local time is " << Clock.get_system_time() << "."
        << std::endl;


    int sleep_millis = 10;
    usleep(sleep_millis * 1000);

    t_ns = Clock.get_ns();
    t_millis = Clock.get_millis();
    t_seconds = Clock.get_seconds();

    std::cout << "\nManual delay of 10 ms:\n"
        << "Nanos" << t_ns 
        << "\tMillis = " << t_millis 
        << "\tSeconds = " << t_seconds
        << std::endl;


    sleep_millis = 90;
    usleep(sleep_millis * 1000);

    t_ns = Clock.get_ns();
    t_millis = Clock.get_millis();
    t_seconds = Clock.get_seconds();
    
    std::cout << "\nManual delay of additional 90 ms (total 100 ms):\n"
        << "Nanos = " << t_ns
        << "\tMillis = " << t_millis
        << "\tSeconds = " << t_seconds
        << std::endl;

    std::cout << "\nRunning timer mode:" << std::endl;
    sleep_millis = 100;
    double dt = 0.0;
    double dt_millis = 0.0;

    Clock.reset();
    Clock.init_timer();
    for (int i = 0; i < 5; i++)
    {
        usleep(sleep_millis * 1000);
        dt = Clock.get_dt();
        dt_millis = Clock.get_dt_millis();
        std::cout << "Run " << i << " :" << dt << " seconds or "
            << dt_millis << " ms"
            << std::endl;    
    }
    std::cout << "Total run time: " 
        << Clock.get_seconds() << " seconds or " 
        << Clock.get_millis() << " ms" 
        << Clock.get_ns() << " ns" 
        << std::endl;

    Clock.reset();
    Clock.init_timer();
    for (int i = 0; i < 1000; i++)
    {
        usleep(sleep_millis * 1000);
        dt = Clock.get_dt();
        dt_millis = Clock.get_dt_millis();
        std::cout << "Run " << i << " :" << dt << " seconds or "
            << dt_millis << " ms"
            << std::endl;    
    }
    std::cout << "Total run time: " 
        << Clock.get_seconds() << " seconds or \n" 
        << Clock.get_millis() << " ms or \n" 
        << Clock.get_ns() << " ns" 
        << std::endl;

    return 0;
}
