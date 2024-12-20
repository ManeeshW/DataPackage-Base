# FDCL Save Class

<a name="contents"></a>
## Contents
1. [Basic Introduction](#intro)
2. [Using the Class](#using-the-class)
3. [Contributing](#contributing)

<a name="intro"></a>
## Basic Introduction

This class is used for easily saving data into a file on a hard disk with C++. 
Below is an example on how to use this class.

```
#include "fdcl/save.hpp"


int main(void)
{
    // initialize the class first
    fdcl::save log;


    // define few variables that needs to be written
    int i = 0;
    double d = M_PI;

    Eigen::Matrix<double, 3, 1> J;
    J.setIdentity();


    // open the file	
    log.open("test_fdcl_save.txt");


    // write data
    log.write(d);
    log.write(i);

    // write a new line character
    log.endl();

    // write more data
    log.write(d);
    log.write(J);

    // write another new line character to the file
    log.endl();
	
	
    // save data to the hard drive
    // skipping this step may not write any data
    log.close();
}
```
[back to contents](#contents)

<a name="using-the-class"></a>
## Using the Class

To add this to class to your code, using CMake and cloning it as a git submodule is recommended. 
This instructions assumes that you are going to add your submodules to a directory named `libraries` in the main project directory.
If your submodule directory is different, make sure to change the path wherever it says `libraries`.
First, add this as a submodule in git.

```
cd 'main/project/dir'
git submodule add https://github.com/fdcl-gwu/fdcl_save.git ./libraries/fdcl_save
git submodule update --init --recursive
```

NOTE: Whenever you clone your main project, you must recursively update the submodules:
```
git submodule update --init --recursive
```

Now, in the main project's CMake file (CMakeLists.txt), do the followings:
```
include_directories(${PROJECT_SOURCE_DIR}/libraries/fdcl_save/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/libraries/fdcl_save fdcl_save)
```

Also, whenever you make a file that uses fdcl_save class, add `fdcl_save` to the linker:
```
target_link_libraries(name/of/the/library/or/exec
    PRIVATE fdcl_save
)
```

Then, you can simply call `#include "fdcl/save.hpp"` in your source/header files in the main directory.

<a name="contributing"></a>
## Contributing
* Anyone is welcome to contribute, but make sure you follow the existing coding style.
* Make sure to document all your changes/additions with Doxygen style comments.

### Generating the Documentation
Document generation is done with Doxygen
If you do not have Doxygen, install it first
```
sudo apt-get install -y doxygen graphviz
```

Use Doxygen to generate the documentation
```
cd docs/Doxygen
doxygen Doxygen
```

This will generate the documentation. 
Commit and push to update the online documentation.
[back to contents](#contents)
