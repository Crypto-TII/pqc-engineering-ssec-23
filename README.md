# README

## Let us walk on the 3-isogeny graph: efficient, fast, and simple

## Setup Process

This project uses `cmake` and it has been configured to run with clion. A stand-alone `cmake` build can be done as well, but the instructions have to be run manually.

To set up the environment, in clion, create four different cmake profiles with the following instructions:

- Debug-cycles-x8664:
  - Build type: Use `Debug`.
  - Generator: Use `Unix Makefiles`.
  - CMake Options: Use `-G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DBENCHMARKING=CYCLES -DARCHITECTURE=x8664`

- Release-cycles-x8664:
  - Build type: Use `Release`.
  - Generator: Use `Unix Makefiles`.
  - CMake Options: Use `-G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DBENCHMARKING=CYCLES -DARCHITECTURE=x8664`

- Debug-time-x8664:
  - Build type: Use `Debug`.
  - Generator: Use `Unix Makefiles`.
  - CMake Options: Use `-G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DBENCHMARKING=TIME -DARCHITECTURE=x8664`

- Release-cycles-x8664:
  - Build type: Use `Release`.
  - Generator: Use `Unix Makefiles`.
  - CMake Options: Use `-G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DBENCHMARKING=TIME -DARCHITECTURE=x8664`

Please refer to the figure below for clarification.

![CMake Profile](gifs/01-cmake-profile/cmake-profile.gif)

## Doxygen

To generate the Doxygen documentation of the project, please run

```bash
ssec-23/docs$ doxygen Doxyfile
```

## Detailed build process

Please refer to [c-code/README.md](c-code/README.md) for a detailed description of the build process of the c code. To build the modified dCTIDH, please refer to [README.md](dCTIDH/README.md).
