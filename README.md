# INF273 Pickup and Delivery Project

## Building
First configure CMake which you should probably do in a separate folder. When configuring CMake you can also specity a compiler to use (default compiler will be used unless specified). Here's an example with Visual Studio:
```
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
```

After configuring you can either build it from the compiler chosen or use CMake to initiate building:
```
cmake --build . --config Debug
```
The finished binary will end up in the `bin` folder under the root folder. The data is loaded as relative paths from the `bin` folder, but the program also takes relative paths to the data as arguments. Heres two examples for Windows with Visual Studio:

*From the bin folder:*
```
./Debug/pickup_and_delivery.exe
```
*From the root folder:*
```
./bin/Debug/pickup_and_delivery.exe bin/data/Call_7_Vehicle_3.txt bin/data/Call_18_Vehicle_5.txt bin/data/Call_035_Vehicle_07.txt bin/data/Call_080_Vehicle_20.txt bin/data/Call_130_Vehicle_40.txt
```

*(In Windows you can also drag and drop the data.txt file onto the executable itself. :o)*