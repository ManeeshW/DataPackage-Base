#ifndef FDCL_TYPES_FREQ_HPP
#define FDCL_TYPES_FREQ_HPP


namespace fdcl {


/** \brief Data structure to save time related data for sensors
 */
struct freq_t {
    double t = 0.0;  /**< Time from the start of the sensor in the current 
        * loop in seconds.
        */
    double t_pre = 0.0;  /**<  Time from the start of the sensor in the previous 
        * loop in seconds.
        */

    /** \brief Calculate the frequemcy using the current t and the t_pre which
     * is the system time at the last time the t was updated.
     */
    int calculate_freq(const double t_current) {
        t_pre = t;
        t = t_current;

        double dt = t - t_pre;

        // Limit the value of dt such that the frequency calculation does not 
        // go to infinity.
        if (dt < 1e-20) {
            dt = 1e-20;
        }

        return (int) (1.0 / dt);
    }

    /** \brief Return the time difference between the current time and the 
     * previous time where the t was updated.
     */
    double get_dt(void) {
        return t - t_pre;
    }
};

}  // end of namespace fdcl

#endif