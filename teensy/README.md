#CRITICAL IMPORTANT SERIOUS INFO

These modified files (AudioStream.h and AudioStream.cpp) are required for proper operation of the Noise Plethora. They should (*must*) replace the originals installed with Teensyduino.

Seriously.

What do they do? The Teensy Audio Library assumes essentially static operation: you load your sketch, AudioStream objects and AudioConnector patch cords are instantiated and the thing runs. There's no facility in place for dynamic loading and unloading of plugins, like what we're doing for the Noise Plethora.

These files add that capability. After (*some very difficult*) troubleshooting, additional changes have been made to ensure interrupt safety when swapping modules in and out at runtime. So far, so good, and it seems stable now.

So please replace the files, at something like `/Applications/Teensyduino.app/Contents/Java/hardware/teensy/avr/cores/teensy4` (on macOS), or `/arduino-1.8.13/hardware/teensy/avr/cores/teensy4` (on Linux), and something similar on Windows.

Also, you'll need to replace the Delay effect header, also provided in the folder hierarchy. You'll figure it out.