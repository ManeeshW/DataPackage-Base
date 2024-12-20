#include "fdcl/save.hpp"


fdcl::save::save()
{
    // Make sure the bool buf_line_full is false at the beginning.
    // If the size of the data sent to write is greater then the MAX_BUF_SIZE
    // defined in the header file, the code is going to complain.
    // If that happens, just increase the data buffer size by changing the
    // MAX_BUF_SIZE.
    buf_line_full = false;

    // This sets the initial location of the data in the data buffer
    buf_line_loc = 0;

    // If a name is not defined, the data will be saved to a file named
    // 'log.txt'.
    file_name = "log.txt";
}


fdcl::save::save(std::string file_name)
{
    buf_line_loc = 0;
    this->file_name = file_name;
    fdcl::save::open(file_name);
}


fdcl::save::~save()
{
    fd.close();
}


void fdcl::save::open(std::string file_name)
{
    this->file_name = file_name;
    fd.open(file_name.c_str());
    std::cout << "SAVE: '" << file_name << "' opened" << std::endl;
}


void fdcl::save::write(std::string s)
{
    char *cstr = &s[0u];
    if(!buf_line_full)
        buf_line_loc += snprintf(buf_line + buf_line_loc,
            MAX_BUF_LINE - buf_line_loc, "%s, ", cstr);

    fdcl::save::is_buf_line_full();
}


void fdcl::save::write(double d)
{
    if(!buf_line_full)
        buf_line_loc += snprintf(buf_line + buf_line_loc,
            MAX_BUF_LINE - buf_line_loc, "%e, ", d);

    fdcl::save::is_buf_line_full();
}


void fdcl::save::write(float f)
{
    if(!buf_line_full)
        buf_line_loc += snprintf(buf_line + buf_line_loc,
            MAX_BUF_LINE - buf_line_loc, "%e, ", f);

    fdcl::save::is_buf_line_full();
}


void fdcl::save::write(int i)
{
    if(!buf_line_full)
        buf_line_loc += snprintf(buf_line + buf_line_loc,
            MAX_BUF_LINE - buf_line_loc, "%d, ", i);

    fdcl::save::is_buf_line_full();
}


template<typename Derived>
void fdcl::save::write(Eigen::MatrixBase<Derived> &M)
{
    int i, j;
    for(i = 0; i < M.rows(); i++)
    {
        for(j = 0; j < M.cols(); j++)
        {
            if(!buf_line_full)
                buf_line_loc += snprintf(buf_line + buf_line_loc,
                    MAX_BUF_LINE - buf_line_loc, "%e, ", M(i, j));

            fdcl::save::is_buf_line_full();
        }
    }
}


void fdcl::save::endl()
{
    if(!buf_line_full)
        buf_line_loc += snprintf(buf_line + buf_line_loc,
            MAX_BUF_LINE - buf_line_loc, "\n");

    fdcl::save::is_buf_line_full();

    // save the line to the stringstream buffer and start a new line
    if(!buf_line_full) buf.write(buf_line, buf_line_loc);
    else buf.write(buf_line, MAX_BUF_LINE);

    buf_line_loc = 0;
}


void fdcl::save::close()
{
    fd << buf.str();
    fd.close();

    std::cout << "SAVE: saved to '" << file_name << "'"<< std::endl;
}


void fdcl::save::is_buf_line_full()
{
    if (buf_line_loc > MAX_BUF_LINE)
    {
        std::cout << "SAVE: buffer truncated, increase MAX_BUF_LINE at "
                  << "fdcl_save.h at least to " << buf_line_loc
                  << std::endl;
        buf_line_full = true;
    }
}

template void fdcl::save::write(Eigen::MatrixBase<Eigen::Matrix<double, 3, 1>>
        & M);
template void fdcl::save::write(Eigen::MatrixBase<Eigen::Matrix<double, 3, 3>>
        & M);
template void fdcl::save::write(Eigen::MatrixBase<Eigen::Matrix<double, 4, 1>>
        & M);
template void fdcl::save::write(Eigen::MatrixBase<Eigen::Matrix<double, 4, 3>>
        & M);
template void fdcl::save::write(Eigen::MatrixBase<Eigen::Matrix<double, 6, 1>>
        & M);
template void fdcl::save::write(Eigen::MatrixBase<Eigen::Matrix<double, 7, 3>>
            & M);
template void fdcl::save::write(Eigen::MatrixBase<Eigen::Matrix<double, 8, 1>>
            & M);
