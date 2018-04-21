
ROW   : x-axis (camera) : move sideways
PITCH : y-axis          : move forward, backward
YAW   : z-axis          : turning

desired : 80cm
actual  : ??cm

marker size : 12.3cm

move3D takes speed

rvecs and tvecs are the rotation and translation vectors

x: red, y: green, z: blue // plane of paper

we will use Z for measuring distance between marker and drone

use the marker detection to get actual distance
error = actual distance - desired distance

## PIDManager

getCommand(error, output)
