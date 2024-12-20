#ifndef FDCL_TYPES_PIKSI_HPP
#define FDCL_TYPES_PIKSI_HPP

#include "libsbp/sbp.h"


namespace fdcl {


/** \brief Piksi message structure
* Data structure for saving different GPS messages
*/

struct piksi_t {
    // GPS solution data
    double lat = 0.0;  /**< Latitude [deg] */
    double lon = 0.0;  /**< Longitude [deg] */
    double h = 0.0; /**< altitude [m] */
    float S_llh_h = 0.0; /**< Horizontal position estimated std deviation [m] */
    float S_llh_v = 0.0; /**< Vertical position estimated std deviation [m] */

    // RTK data
    double n = 0.0; /**< Baseline North [m] */
    double e = 0.0; /**< Baseline East [m] */
    double d = 0.0; /**< Baseline Down [m] */
    float S_rtk_x_h = 0.0;  /**< Baseline horizontal std deviation [m]*/
    float S_rtk_x_v = 0.0;  /**< Baseline vertical std deviation [m] */

    double v_n = 0.0; /**< Baseline velocity North [m/s] */
    double v_e = 0.0; /**< Baseline velocity East [m/s] */
    double v_d = 0.0; /**< Baseline velocity Down [m/s] */
    float S_rtk_v_h = 0.0; /**< Velocity horizontal std deviation [m]*/
    float S_rtk_v_v = 0.0; /**< Velocity vertical std deviation [m] */

    // GPS time
    u8 hr = 0, min = 0, sec = 0; /**< GPS time data */
    double ms = 0.0; /**< GPS nano millisecond */
    float utc = 0.0; /**< UTC */

    // GPS solution data
    int sats = 0; /**< Number of satellites in the vicinity */

    int status = 0; /**< RTK status flag \n
    * 0 Invalid\n
    * 1 Single Point Position (SPP)\n
    * 2 Differential GNSS (DGNSS)\n
    * 3 Float RTK\n
    * 4 Fixed RTK\n
    * 5 Dead reckoning\n
    * 6 SBAS Position\n */

    bool rtk_solution = false; /**< True if an RTK solution exists */
};
}  // end of namespace fdcl

#endif