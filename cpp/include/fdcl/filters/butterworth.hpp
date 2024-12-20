/** \file filter_butterworth.hpp
*  \brief Butterworth filter class
*/

#ifndef FDCL_FILTER_BUTTERWORTH_HPP
#define FDCL_FILTER_BUTTERWORTH_HPP

#include "fdcl/types/vectors.hpp"


namespace fdcl {

namespace filters {

/** \brief First order Butterworth filter for filtering vector3
 */
class Butterworth {
public:
    Butterworth(void);
    ~Butterworth(void);


    /** \fn void set_parameters(vector2 a_in, vector2 b_in)
     * @brief Set the parameters of the first order Butterworth filter
     * 
     * @param a_in components of denominator
     * @param b_in components of numerator
     */
    void set_parameters(vector2 a_in, vector2 b_in);

    /** \fn vector3 update(vector3 new_data)
     * @brief Update the filter with new data and returns the new
     * filtered data.
     * 
     * @param new_data  new data input to the filter
     */
    vector3 update(vector3 new_data);


    /** \fn vector2 get_a(void)
     * @brief Returns the values saved in a.
     * 
     * @return vector2 the values saved in a
     */
    vector2 get_a(void);


    /** \fn vector2 get_b(void)
     * @brief Returns the values saved in b.
     * 
     * @return vector2 the values saved in b
     */
    vector2 get_b(void);


    /** \fn void set_pre_data(vector3 pre_data_in
     * @brief Sets the data in the previous loop.
     * 
     * This is used for veryfing purposes, not needed to set this in
     * regular implementations.
     * 
     */
    void set_pre_data(vector3 pre_data_in);


private:
    vector2 a;  /**< components of denominator */
    vector2 b;  /**< components of numerator */
    vector3 filtered;  /**< filtered data */
    vector3 pre_data;  /**< raw data of the previous loop */
};

}  // end of namespace filters
}  // end of namespace fdcl

#endif