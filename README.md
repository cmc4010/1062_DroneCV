# Computer Vision for UAV Autopilot

## Project setup

### Windows (Visual Studio 2013)

##### System Settings

1. Download
2. Extract it to a proper directory
  - `Example: “C:\opencv”`
3. Add OpenCV to system PATH variable
  1. Navigate to advanced system properties
  2. Click on Environment Variables
  3. Find “path” variable and add the location of OpenCV’s binary
    - `Example: “C:\opencv\build\x64\vc12\bin"`
4. Restart computer

##### Project Setup
**Important:** Perform step #1 before adding any code/file to the project to avoid any problems.

1. Set x64 platform through Configuration Manager
2. VC++ Directories
  1. Add the OpenCV “include” directory to “Include Directories”
    1. `Example: “C:\opencv\build\include”`
  2. Add the OpenCV “lib” directory to “Library Directories”
    1. `Example: “C:\opencv\build\x64\vc12\lib”`
3. C/C++ → General
  1. Add "include” directories to “Additional Include Directories”
    1. `“C:\opencv\build\include”`
    2. `“C:\opencv\build\include\opencv”`
    3. `“C:\opencv\build\include\opencv2”`
4. Linker → General
  1. Add “lib” directory to “Additional Library Directories”
    1. `Example: “C:\opencv\build\x64\vc12\lib”`
5. Linker → Input
  1. Add specific “lib” files to “Additional Dependencies”
    1. `“opencv_world300.lib”`
    2. `“opencv_world300d.lib”`

### macOS

1: Install [Homebrew](https://brew.sh/)

2: Install OpenCV

`brew install opencv`

3: Compile your code

`g++ $(pkg-config --cflags --libs opencv) main.cpp -o main`
