# ONR Data Package

## Dependencies (C++)
### Base
```sh
sudo apt install python3-pip
sudo apt install cmake libgtkmm-3.0-dev # if Linux base
sudo apt install libserialport-dev

brew install gtkmm3 pygobject3 # if Mac base
```

##### Add your user to the dialout group:
```sh
sudo usermod -aG dialout $(whoami)
groups
```
##### Log out and log back in (or reboot) for the changes to take effect.

### Rover
```sh
sudo apt install cmake
sudo apt install build-essential
sudo apt install doxygen

# SBP library
cd libraries/libsbp/c
mkdir build
cd build
cmake ../
make
sudo make install


git clone --branch v2.7.4 https://github.com/swift-nav/libsbp.git
git submodule update --init
cd libsbp/
git submodule update --init
cd c/
mkdir build
cd build/
cmake ../
make
sudo make install
cd ..
cd python/
pip install .
```
#### new update
```sh
nm-connection-editor
git clone --branch v2.7.4 https://github.com/swift-nav/libsbp.git
git submodule update --init
cd libsbp/
git submodule update --init
cd c/
mkdir build
cd build/
cmake ../
make
sudo make install
cd ..
cd python/
pip install .
```

1. Install `libserialport`: [https://github.com/fdcl-gwu/libserialport](https://github.com/fdcl-gwu/libserialport)

## Dependencies (Python)
### Mac
```sh
conda create --name python39 python==3.9
conda activate python39

brew install gtk+3 pygobject3

pip3 install pgi
pip3 install pycairo
pip3 install numpy
pip3 install matplotlib
pip3 install pandas

pip3 install sbp

conda install protobuf

```




### Rover
```sh
sudo pip3 install adafruit-circuitpython-bno055
sudo pip3 install Adafruit-Blinka
pip3 install sbp

sudo apt install -y protobuf-compiler
```

## Compiling proto message
```sh
protoc data.proto --python_out=./proto
```

## Troubleshooting
* ModuleNotFoundError: No module named 'gi'
```sh
export PYTHONPATH=/usr/local/lib/python3.9/site-packages/ 
```

## Alias
```bash
alias gtbp="cd onr-data-package/scripts/"
alias rbp="python3 base.py"
```