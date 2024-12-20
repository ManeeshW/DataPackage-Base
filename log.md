maneesh@ubuntu:~/onr-data-package/cpp/build$ make rover
[  2%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/attitude.cpp.o
[  4%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/compositedata.cpp.o
[  6%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/conversions.cpp.o
[  8%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/criticalsection.cpp.o
[ 10%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/dllvalidator.cpp.o
[ 13%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/error_detection.cpp.o
[ 15%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/event.cpp.o
[ 17%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/ezasyncdata.cpp.o
[ 19%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/memoryport.cpp.o
[ 21%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/packet.cpp.o
[ 23%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/packetfinder.cpp.o
[ 26%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/port.cpp.o
[ 28%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/position.cpp.o
[ 30%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/searcher.cpp.o
[ 32%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/sensors.cpp.o
[ 34%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/serialport.cpp.o
[ 36%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/thread.cpp.o
[ 39%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/types.cpp.o
[ 41%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/util.cpp.o
[ 43%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/utilities.cpp.o
[ 45%] Building CXX object libvncxx/CMakeFiles/libvncxx.dir/src/vntime.cpp.o
[ 47%] Linking CXX static library liblibvncxx.a
[ 47%] Built target libvncxx
[ 50%] Building CXX object fdcl_save/CMakeFiles/fdcl_save.dir/src/save.cpp.o
[ 52%] Linking CXX static library libfdcl_save.a
[ 52%] Built target fdcl_save
[ 54%] Building CXX object fdcl_param/CMakeFiles/fdcl_param.dir/src/param.cpp.o
[ 56%] Linking CXX static library libfdcl_param.a
[ 56%] Built target fdcl_param
[ 58%] Building CXX object fdcl_serial/CMakeFiles/fdcl_serial.dir/src/serial.cpp.o
[ 60%] Linking CXX static library libfdcl_serial.a
[ 60%] Built target fdcl_serial
[ 63%] Building CXX object fdcl_clock/CMakeFiles/fdcl_clock.dir/src/fdcl_clock.cpp.o
[ 65%] Linking CXX static library libfdcl_clock.a
[ 65%] Built target fdcl_clock
[ 67%] Building CXX object CMakeFiles/rover_libs.dir/src/fdcl/system.cpp.o
[ 69%] Building CXX object CMakeFiles/rover_libs.dir/src/fdcl/wifi.cpp.o
[ 71%] Building CXX object CMakeFiles/rover_libs.dir/src/fdcl/sensors/anemometer.cpp.o
[ 73%] Building CXX object CMakeFiles/rover_libs.dir/src/fdcl/sensors/vn100.cpp.o
[ 76%] Building CXX object CMakeFiles/rover_libs.dir/src/fdcl/sensors/swiftnav.cpp.o
/home/maneesh/onr-data-package/cpp/src/fdcl/sensors/swiftnav.cpp: In member function ‘void fdcl::sensors::Swiftnav::init_loop()’:
/home/maneesh/onr-data-package/cpp/src/fdcl/sensors/swiftnav.cpp:66:26: error: invalid conversion from ‘u32 (*)(u8*, u32, void*)’ {aka ‘unsigned int (*)(unsigned char*, unsigned int, void*)’} to ‘s32 (*)(u8*, u32, void*)’ {aka ‘int (*)(unsigned char*, unsigned int, void*)’} [-fpermissive]
   66 |         sbp_process(&s0, &piksi_port_read);
      |                          ^~~~~~~~~~~~~~~~
      |                          |
      |                          u32 (*)(u8*, u32, void*) {aka unsigned int (*)(unsigned char*, unsigned int, void*)}
In file included from /home/maneesh/onr-data-package/cpp/include/fdcl/types/piksi.hpp:4,
                 from /home/maneesh/onr-data-package/cpp/include/fdcl/sensors/swiftnav.hpp:7,
                 from /home/maneesh/onr-data-package/cpp/src/fdcl/sensors/swiftnav.cpp:1:
/home/maneesh/onr-data-package/cpp/../libraries/libsbp/c/include/libsbp/sbp.h:146:38: note:   initializing argument 2 of ‘s8 sbp_process(sbp_state_t*, s32 (*)(u8*, u32, void*))’
  146 | s8 sbp_process(sbp_state_t *s, s32 (*read)(u8 *buff, u32 n, void* context));
      |                                ~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/maneesh/onr-data-package/cpp/src/fdcl/sensors/swiftnav.cpp: In member function ‘void fdcl::sensors::Swiftnav::loop()’:
/home/maneesh/onr-data-package/cpp/src/fdcl/sensors/swiftnav.cpp:74:27: error: invalid conversion from ‘u32 (*)(u8*, u32, void*)’ {aka ‘unsigned int (*)(unsigned char*, unsigned int, void*)’} to ‘s32 (*)(u8*, u32, void*)’ {aka ‘int (*)(unsigned char*, unsigned int, void*)’} [-fpermissive]
   74 |     ret = sbp_process(&s, &piksi_port_read);
      |                           ^~~~~~~~~~~~~~~~
      |                           |
      |                           u32 (*)(u8*, u32, void*) {aka unsigned int (*)(unsigned char*, unsigned int, void*)}
In file included from /home/maneesh/onr-data-package/cpp/include/fdcl/types/piksi.hpp:4,
                 from /home/maneesh/onr-data-package/cpp/include/fdcl/sensors/swiftnav.hpp:7,
                 from /home/maneesh/onr-data-package/cpp/src/fdcl/sensors/swiftnav.cpp:1:
/home/maneesh/onr-data-package/cpp/../libraries/libsbp/c/include/libsbp/sbp.h:146:38: note:   initializing argument 2 of ‘s8 sbp_process(sbp_state_t*, s32 (*)(u8*, u32, void*))’
  146 | s8 sbp_process(sbp_state_t *s, s32 (*read)(u8 *buff, u32 n, void* context));
      |                                ~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make[3]: *** [CMakeFiles/rover_libs.dir/build.make:132: CMakeFiles/rover_libs.dir/src/fdcl/sensors/swiftnav.cpp.o] Error 1
make[2]: *** [CMakeFiles/Makefile2:221: CMakeFiles/rover_libs.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:198: CMakeFiles/rover.dir/rule] Error 2
make: *** [Makefile:124: rover] Error 2




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