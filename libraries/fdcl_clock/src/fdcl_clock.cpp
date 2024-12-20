#include "fdcl/clock.hpp"


fdcl::Clock::Clock(void)
{
    this->init();
}


fdcl::Clock::Clock(std::string clock_name) : clock_name_(clock_name)
{
    this->init();
}


fdcl::Clock::~Clock(void)
{
    ;
}


void fdcl::Clock::init(void)
{
    std::cout << "Initalizing " << clock_name_ << " ..." << std::endl;
    this->reset();
}


void fdcl::Clock::reset(void)
{
    clock_gettime(CLOCK_REALTIME, &tspec_init_);

    t0_seconds_ = (double)tspec_init_.tv_sec \
        + ((double)tspec_init_.tv_nsec) / 1.0e9;
    t0_millis_ = t0_seconds_ * 1.0e3;
    t0_ns_ = t0_seconds_ * 1.0e9;
}


unsigned long long fdcl::Clock::get_ns(void)
{
    unsigned long long t;
    clock_gettime(CLOCK_REALTIME, &tspec_curr_);
    t = ((unsigned long long)tspec_curr_.tv_sec * 1.0e9 \
        + ((unsigned long long)tspec_curr_.tv_nsec));
    t -= t0_ns_;

    return t;
}


double fdcl::Clock::get_millis(void)
{
    double t;
    clock_gettime(CLOCK_REALTIME, &tspec_curr_);
    t = ((double)tspec_curr_.tv_sec \
        + ((double)tspec_curr_.tv_nsec) / 1.0e9) * 1.0e3;
    t -= t0_millis_;

    return t;
}


void fdcl::Clock::init_timer(void)
{
    t_prev_ = this->get_seconds();
    t_prev_millis_ = this->get_millis();
}


double fdcl::Clock::get_dt(void)
{
    double t = this->get_seconds();
    double dt = t - t_prev_;
    t_prev_ = t;
    return dt;
}


double fdcl::Clock::get_dt_millis(void)
{
    double t = this->get_millis();
    double dt = t - t_prev_millis_;
    t_prev_millis_ = t;
    return dt;
}


double fdcl::Clock::get_seconds(void)
{
    double t;
    clock_gettime(CLOCK_REALTIME, &tspec_curr_);
    t = (double)tspec_curr_.tv_sec + ((double)tspec_curr_.tv_nsec) / 1.0e9;
    t -= t0_seconds_;

    return t;
}


std::string fdcl::Clock::get_system_time(void)
{
    time_t now = time(0);

    struct tm tstruct;
    char current_time[80];

    tstruct = *localtime(&now);
    strftime(current_time, sizeof(current_time), "%H%M%S", &tstruct);

    return current_time;
}


std::string fdcl::Clock::get_date_time(void)
{
    time_t now = time(0);

    struct tm tstruct;
    char current_time[80];

    tstruct = *localtime(&now);
    strftime(current_time, sizeof(current_time), "%Y%m%d_%H%M%S", &tstruct);

    return current_time;
}


std::string fdcl::Clock::get_date(void)
{
    time_t now = time(0);

    struct tm tstruct;
    char current_time[80];

    tstruct = *localtime(&now);
    strftime(current_time, sizeof(current_time), "%Y%m%d", &tstruct);

    return current_time;
}