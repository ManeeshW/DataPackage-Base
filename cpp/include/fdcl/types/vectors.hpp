/** \file types.hpp
 *  \brief Data types used throughout the code.
 *
 * Rype definitions used are defined here.
 */

#ifndef FDCL_TYPES_HPP
#define FDCL_TYPES_HPP

// external libraries
#include "Eigen/Dense"


namespace fdcl {
    
// Common eigen matrix definition used throughout the code
typedef Eigen::Matrix<double, 2, 1> vector2;
typedef Eigen::Matrix<double, 3, 1> vector3;
typedef Eigen::Matrix<double, 4, 1> vector4;
typedef Eigen::Matrix<double, 6, 1> vector6;
typedef Eigen::Matrix<double, 7, 1> vector7;
typedef Eigen::Matrix<double, 8, 1> vector8;

typedef Eigen::Matrix<double, 3, 3> matrix3;
typedef Eigen::Matrix<double, 6, 6> matrix6;
typedef Eigen::Matrix<double, 7, 7> matrix7;

} // namespace fdcl

#endif
