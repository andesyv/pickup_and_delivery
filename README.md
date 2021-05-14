# INF273 Pickup and Delivery Project

Project for the course project for the INF273 course: Meta-Heuristics, at the University of Bergen. Meta-Heuristic is about optimization, so this project features a variety of algorithms made to find a good solution to the NP-Hard problem "pickup and delivery".

In this particular instance of "pickup and delivery", we had a scenario with a number of delivery vehicles and a bunch of orders, and we were tasked with finding the most effective routes. Each file in the *data* folder specifies such a problem where the calls represents the orders the vehicles are tasked with shipping. Each file describes the calls available, the vehicles the costs of each vehicle transporting a call, and what calls can be delivered by specific vehicles.
## Building
The project uses CMake and some C++20 features, so you will need a C++20 compatible compiler. You could create the build files in a separate folder or in the root folder, so this first step is *optional*:
```
mkdir build
cd build
```

Then run the CMake command on the root folder (`cmake .` if building from root folder). You can supply numerous options which are project specific and platform specific.

The project specific options are as follows:
| Variable              | Values | Default | Description |
| --------------------- | ------ | ------- | ----------- |
| PARALLEL_EXECUTION    | ON/OFF | ON      | Whether to run 10 runs of each instance in parallel using multi-threading. |
| ALL_ALGORITHMS        | ON/OFF | OFF     | Whether to run all algorithms or just the final one. (**Currently broken**) |
| FILE_OUTPUT           | ON/OFF | ON      | Whether to output the results into a *output.csv* file and a *solutions.txt* file |
| RUN_FOR_10_MINUTES    | ON/OFF | ON      | Whether to run the program for 10 minutes or stop at the earliest convenience. |

Heres an example for Windows that uses Visual Studio 16 to compile and leaves the values as their default, and builds in release mode:
```
cmake .. -G "Visual Studio 16 2019" -DPARALLEL_EXECUTION=ON -DALL_ALGORITHMS=OFF -DFILE_OUTPUT=ON -DRUN_FOR_10_MINUTES=ON -DCMAKE_BUILD_TYPE=RELEASE
```

Here's an example for linux:
```
cmake .. -DCMAKE_CXX_COMPILER=g++-10 -DPARALLEL_EXECUTION=ON -DALL_ALGORITHMS=OFF -DFILE_OUTPUT=ON -DRUN_FOR_10_MINUTES=ON -DCMAKE_BUILD_TYPE=RELEASE
```
The `-G "Visual Studio 16 2019"` on Windows specifies a "kit", but on Linux you only have to specify the compiler directly: `-DCMAKE_CXX_COMPILER=g++-10`

After configuring you can either build it from the compiler chosen or use CMake to initiate building:
```
cmake --build .
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