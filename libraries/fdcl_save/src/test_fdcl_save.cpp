#include "fdcl/save.hpp"


int main(void)
{
    fdcl::save log;
	int i = 0;
	double d = M_PI;
	Eigen::Matrix<double, 3, 1> J;
	J.setIdentity();
	
	log.open("test_fdcl_save.txt");
	
	log.write(d);
	log.write(i);
	log.endl();
	log.write(d);
	log.write(J);
	log.endl();
		
	log.close();
}
