
# Marker and CVDrone

## Important

- Markers and drones have their own coordinate system

## Marker detection

aruco markers

1. generate predefined marker dictionary
2. detect the markers
3. estimate pose

## CVDrone

Grab the code here: https://github.com/puku0x/cvdrone

This codebase is a combination of AR.Drone and OpenCV.

### Prerequisite

1. have OpenCV and ffmpeg
2. connected to the drone via WiFi before running the program

### How to use?

Edit 'main.cpp' in 'cvdrone/src', compile, and then run.

compilation for MacOS,
```
cd cvdrone/build/unix
make
./test.a
```

So...
1. replace 'main.cpp' in 'cvdrone/src' with yours
2. compile
3. run

**Note:** If your program needs any additional files during runtime, put them under 'cvdrone/build/unix'.
