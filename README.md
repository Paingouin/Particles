[![MacOS](https://github.com/Paingouin/Particles/workflows/MacOS/badge.svg)](https://github.com/Paingouin/Particles/actions)
[![Windows](https://github.com/Paingouin/Particles/workflows/Windows/badge.svg)](https://github.com/Paingouin/Particles/actions)
[![Ubuntu](https://github.com/Paingouin/Particles/workflows/Ubuntu/badge.svg)](https://github.com/Paingouin/Particles/actions)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Paingouin/Particles)](https://github.com/Paingouin/Particles/releases)

# Particles

### Features

* Sand

### Install

On windows:
* Need msvc 2017 + conan  + Cmake
* Use build.bat script provided in tools folder

## Optional

Install NSIS https://nsis.sourceforge.io/Download for executable packaging.

## Tests

Run CTest from the build directory, and and pass the desired configuration

````bash
cd build 
ctest -C Release 
````

### Thanks
Thanks to filipdutescu for the modern cmake template : https://github.com/filipdutescu/modern-cpp-template
