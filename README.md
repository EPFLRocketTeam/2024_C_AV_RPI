# Sensors tests
Once in the repository, execute: 
```
mkdir build
cd build
cmake ..
make
```

Then you can either run the tests individually with:
```
sudo ./[NAME_OF_TEST]_test
```

Or run all the tests with:
```
sudo ctest
```

### A workaround to run gdb with sudo:
```
cd /usr/bin
sudo mv gdb gdborig
```
Then make a bash script named gdb:
```
sudo nano gdb
```
And modify it with the following content:
```
#!/bin/sh
sudo gdborig #@
```
Finally make the script runnable
```
sudo chmod 0755 gdb
```
Then you can debug any executable by selecting it in the CMake Tools window, and
clicking on the executable name under debug.
