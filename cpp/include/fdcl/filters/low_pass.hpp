/** \file filter_low_pass.hpp
*  \brief Low-pass filter class
*/

#ifndef FILTER_LOW_PASS_HPP
#define FILTER_LOW_PASS_HPP

#include "fdcl/types/vectors.hpp"


namespace fdcl {

namespace filters {

/** \brief Low-pass filter for filtering vector3
 */
class LowPass {
public:
    LowPass(void);
    ~LowPass(void);

    /** \fn void set_alpha(double alpha_in)
     * Sets the alpha value of the filter to a given value.
     * @Param alpha_in  new value of the alpha
     */
    void set_alpha(double alpha_in);

    /** \fn vector3 update(vector3 new_data)
     * Update the filter with new data and returns the new filtered data.
     * @Param new_data  new data input to the filter
     */
    vector3 update(vector3 new_data);

    /** \fn double get_alpha(void)
     * Returns the current alpha value of the filter.
     */
    double get_alpha(void);


private:
    double alpha = 0.0;  /**< alpha value for the filter */
    vector3 filtered;
};

}  // end of namespace filters
}  // end of namespace fdcl

#endif