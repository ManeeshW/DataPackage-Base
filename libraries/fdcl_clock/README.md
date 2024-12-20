# FDCL-Clock

This is a C++ class to make timing and extracting system time and date easier.

<a name="contents"></a>
## Contents
1. [Quick Start](#quick-start)
2. [Using the Class](#using-the-class)
3. [Contributing](#contributing)

<a name="quick-start"></a>
## Quick Start

### Initialization 
```C++
#include "fdcl/clock.hpp"

fdcl::Clock Clock;  // Simplest way
fdcl::Clock ClockNamed("Main Timer");  // Instantiate with a name
```

### Extracting Date and Time
```C++
std::string date = Clock.get_date();
std::string time = Clock.get_system_time();
```

### Time Elapsed
```C++
#include <unistd.h>

Clock.reset();  // Reset the epoch
usleep(1000);  // Sleep for 1 millisecond

// Get the time since epoch.
double t_millis = Clock.get_millis();
double t_seconds = Clock.get_seconds();
```

### Time Between
```C++
Clock.init_timer();
for (int i = 0; i < 5; i++)
{
    usleep(1000);

    double dt = Clock.get_dt();
    double dt_millis = Clock.get_dt_millis();

    std::cout << "Run " << i << " :" 
        << dt << " s or "
        << dt_millis << " ms"
        << std::endl;    
}
```

A complete example is included in `src/test_fdcl_clock.cpp`.
Detailed documentation is included in `docs/html/index.html`.
To properly view this, you have to clone/download the repository and open this fine in browser.

[back to contents](#contents)

<a name="using-the-class"></a>
## Using the Class

To add this to class to your code, using CMake and cloning it as a git submodule is recommended. 
This instructions assumes that you are going to add your submodules to a directory named `libraries` in the main project directory.
If your submodule directory is different, make sure to change the path wherever it says `libraries`.
First, add this as a submodule in git.

```
cd 'main/project/dir'
git submodule add https://github.com/fdcl-gwu/fdcl-clock.git ./libraries/fdcl-clock
git submodule update --init
```

NOTE: Whenever you clone your main project, you must update the submodules:
```
git submodule update --init
```

Now, in the main project's CMake file (CMakeLists.txt), do the followings:
```
include_directories(${PROJECT_SOURCE_DIR}/libraries/fdcl-clock/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/libraries/fdcl-clock fdcl_clock)
```

Also, whenever you make a file that uses fdcl-clock class, add `fdcl_clock` to the linker:
```
target_link_libraries(name/of/the/library/or/executale
    PRIVATE fdcl_clock
)
```

Then, you can simply call `#include "fdcl/clock.hpp"` in your source/header files in the main directory.

[back to contents](#contents)

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