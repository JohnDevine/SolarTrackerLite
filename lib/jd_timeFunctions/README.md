# Include this header (jd_timeFunctions.hpp) in the main.cpp and only once.
---

# Troubleshooting

### Error "undefined reference"
If you see errors like this `src/main.cpp:155:1: error: 'myTZ' was not declared in this scope`, you included *jd_timeFunctions.h* instead of *jd_timeFunctions.hpp* in main.cpp
See explanation of why below.

# Why *.hpp instead of *.cpp?

Every *.cpp file is compiled separately by a call of the compiler exclusively for this cpp file. These calls are managed by the IDE / make system. In the Arduino IDE the calls are executed when you click on Verify or Upload.
And now our problem with Arduino is: How to set compile options for all *.cpp files, especially for libraries used?
IDE's like Sloeber or PlatformIO support this by allowing to specify a set of options per project. They add these options at each compiler call e.g. -DTRACE.
But Arduino lacks this feature. So the workaround is not to compile all sources separately, but to concatenate them to one huge source file by including them in your source. This is done by e.g. #include "ServoEasing.hpp". 
But why not #include "jd_timeFunctions.cpp"?
Try it and you will see tons of errors, because each function of the *.cpp file is now compiled twice, first by compiling the huge file and second by compiling the *.cpp file separately, like described above. So using the extension cpp is not longer possible, and one solution is to use hpp as extension, to show that it is an included *.cpp file. Every other extension e.g. cinclude would do, but hpp seems to be common sense.