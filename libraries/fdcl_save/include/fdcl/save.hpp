#ifndef FDCL_SAVE_HPP
#define FDCL_SAVE_HPP

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#include "Eigen/Dense"

#define MAX_BUF_LINE 4096



namespace fdcl
{

/** \brief Logging data into a file
*
* This class is used for logging data to an external file. If the variable 
* file_name is not defined, by default, the data will be saved to file
* named 'log.txt'.
*/
class save
{
public:
    save();
    save(std::string file_name);
    ~save();

    std::ofstream fd; /**< the file all the data to be saved */
    std::stringstream buf; /**< buffer log data are saved to until it is
                            *   written to the file
                            */
    std::string file_name; /**< name of the file in which the data to be 
                            * saved
                            */

    /** \fn void open(std::string file_name)
     * Opens the file to save the data
     * @param file_name name of the file
     */
    void open(std::string file_name);

    /** \fn void write(std::string s)
     * Puts a string parameter to the buffer
     * @param std::string parameter to be written
     */
    void write(std::string s);

    /** \fn void write(double d)
     * Puts a double parameter to the buffer
     * @param double parameter to be written
     */
    void write(double d);

    /** \fn void write(float f)
     * Puts a float parameter to the buffer
     * @param float parameter to be written
     */
    void write(float f);

    /** \fn void write(int i)
     * Puts a int parameter to the buffer
     * @param int parameter to be written
     */
    void write(int i);

    /** \fn void write(Eigen::MatrixBase<Derived> &M)
     * Puts a Eigen vector parameter to the buffer
     * @param Eigen::MatrixBase<Derived> parameter to be written
     */
    template<typename Derived> void write(Eigen::MatrixBase<Derived> &M);

    /**
     * Writes the contents in the buffer to the file. This must be called after
     * the end of each line for the buffer to be actually written. The write
     * functions defined in this class actually only saves the data to the
     * buffer.
     */
    void endl();

    /**
     * Closes the file opened for writing the logging data. This must be called
     * after the last call to the void endl() for the saved data file to be
     * accessed, before exiting the program
     */
    void close();

private:
    char buf_line[MAX_BUF_LINE]; /**< buffer for each line */
    int buf_line_loc; /**< current location of the buffer */
    bool buf_line_full; /**< flag to show if the current line buffer is full */


    /** \fn void is_buf_line_full()
     * To check if the current line buffer is full. If the size of the data
     * sent is greater than the defined maximum size, the code is going to
     * complain. If you get this, just increase the data buffer size by 
     * changing the MAX_BUF_LINE variable.
     */
    void is_buf_line_full();
};
}

#endif
