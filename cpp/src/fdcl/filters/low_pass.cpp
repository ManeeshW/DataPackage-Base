#include "fdcl/filters/low_pass.hpp"

fdcl::filters::LowPass::LowPass(void) {
    filtered.setZero();
}


fdcl::filters::LowPass::~LowPass(void) {
    ;  // do nothing
}


void fdcl::filters::LowPass::set_alpha(double alpha_in) {
    alpha = alpha_in;
}


double fdcl::filters::LowPass::get_alpha(void) {
    return alpha;
}


fdcl::vector3 fdcl::filters::LowPass::update(vector3 new_data) {
    filtered = (1.0 - alpha) * filtered + alpha * new_data;
    return filtered;
}