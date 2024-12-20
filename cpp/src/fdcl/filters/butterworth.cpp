#include "fdcl/filters/butterworth.hpp"


fdcl::filters::Butterworth::Butterworth(void) {
    filtered.setZero();
    pre_data.setZero();
    a.setZero();
    b.setZero();
}


fdcl::filters::Butterworth::~Butterworth(void) {
    ;  // do nothing
}


void fdcl::filters::Butterworth::set_parameters(vector2 a_in, vector2 b_in) {
    a = a_in;
    b = b_in;
}


void fdcl::filters::Butterworth::set_pre_data(vector3 pre_data_in) {
    pre_data = pre_data_in;
}


fdcl::vector2 fdcl::filters::Butterworth::get_a(void) {
    return a;
}


fdcl::vector2 fdcl::filters::Butterworth::get_b(void) {
    return b;
}


fdcl::vector3 fdcl::filters::Butterworth::update(vector3 new_data) {
    filtered = - a(1) * filtered + b(0) * new_data + b(1) * pre_data;
    set_pre_data(new_data);
    return filtered;
}