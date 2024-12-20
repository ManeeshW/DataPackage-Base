#ifndef FDCL_CLOCK_HPP
#define FDCL_CLOCK_HPP

#include <time.h>
#include <iostream>
#include <string>


namespace fdcl
{

/** \class  
 * \brief Time and timing class.
 * 
 * This performs time related functions used in FDCL, including getting the
 * system time or date, detemining the time elapsed from a given epoch, and
 * keeping track of time between consecutive loops.
 */
class Clock
{
public:
    /** \fn Clock(void)
     * Instantiate the class with the default name "FDCL::Clock".
     */
    Clock(void);

    /** \fn Clock(std::string clock_name)
     * Instantiate the class with a desired name.
     * @param[in] clock_name Name of the clock.
     */
    Clock(std::string clock_name);

    /** \fn ~Clock(void)
     * Destruct the timer object.
     */
    ~Clock(void);

    /** \fn void reset(void)
     * Re-initialize the class, re-setting the epoch.
     */
    void reset(void);

    /** \fn double get_ns(void)
     * Return the time since epoch, in nanoseconds.
     * \return t Time since epoch, in nanoseconds.
     */
    unsigned long long get_ns(void);

    /** \fn double get_millis(void)
     * Return the time since epoch, in milliseconds.
     * \return t Time since epoch, in milliseconds.
     */
    double get_millis(void);

    /** \fn double get_seconds(void)
     * Return the time since epoch, in seconds.
     * \return t Time since epoch, in seconds.
     */
    double get_seconds(void);

    /** \fn void init_timer(void)
     * Initialize the timer. Timer is used to get the time difference between
     * two consecutive calls of get_dt() or get_dt_millis(). This must be
     * called before using get_dt() or get_dt_millis().
     * This does not reset the epoch.
     */
    void init_timer(void);

    /** \fn double get_dt(void)
     * Return the time elapsed from the last call to get_dt() or 
     * init_timer(), in seconds.
     * \return dt Time elapsed, in seconds.
     */
    double get_dt(void);

    /** \fn double get_dt_millis(void)
     * Return the time elapsed from the last call to get_dt_millis() or 
     * init_timer(), in milliseconds.
     * \return dt Time elapsed, in milliseconds.
     */
    double get_dt_millis(void);

    /** \fn std::string get_date(void)
     * Return the current system date as a string.
     * \return date Local system date as YYYYMMDD.
     */
    std::string get_date(void);

    /** \fn std::string get_system_time(void)
     * Return the current system time as a string.
     * \return date Local system date as HHMMSS.
     */
    std::string get_system_time(void);

    /** \fn std::string get_date_time(void)
     * Return the current system date and time as a string.
     * \return date Local system date as YYYYMMDD_HHMMSS.
     */
    std::string get_date_time(void);

private:
    struct timespec tspec_init_;  /**< Initial time data. */
    struct timespec tspec_curr_;  /**< Current time data. */
    
    std::string clock_name_ = "FDCL::Clock";  /**< Name of the clock. */

    unsigned long long t0_ns_ = 0;  /**< Initial time, which is set when the
        *  class is initialized or reset, in nanoseconds.
        */
    double t0_millis_ = 0.0;  /**< Initial time, which is set when the class
        * is initialized or reset, in milliseconds.
        */
    double t0_seconds_ = 0.0;  /**< Initial time, which is set when the class
        * is initialized or reset, in seconds.
        */

    double t_prev_ = 0.0;  /**< Time at the previous time when the get_dt is 
        * called, in seconds.
        */
    double t_prev_millis_ = 0.0;  /**< Time at the previous time when the 
        * get_dt_millis is called, in milliseconds.
        */

    /** \fn void init(void)
     * Initialize the class.
     */
    void init(void);
};

} // End of namespace fdcl
#endif