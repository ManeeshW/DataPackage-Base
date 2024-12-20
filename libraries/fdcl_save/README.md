# FDCL Save Class

This class is used for easily saving data into a file on a hard disk. 
Currently, the class supports saving `int`, `float`, `double`, `std::string` and eigen vectors/matrices of size 3x1, 4x1, and 3x3.

Check [full documentation](https://fdcl-gwu.github.io/fdcl_save/html/index.html) for more details.


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
