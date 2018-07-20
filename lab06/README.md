
# Motion Planning

Drone coordinate system
ROW   : x-axis (camera) : move sideways
PITCH : y-axis          : move forward, backward
YAW   : z-axis          : turning

desired : 80cm
actual  : ??cm (determined using pose estimation with aruco marker)

marker size : 12.3cm

move3D takes speed

## Error approximation

rvecs and tvecs returned by pose estimation are the rotation and translation vectors

Aruco marker coordinate system
x: red, y: green, z: blue // plane of paper

we will use the z value for measuring distance between marker and drone

use the marker detection to get actual distance
error = actual distance - desired distance

## Understanding PID

Proportional Integral Differential (PID)

Proportional Term : Kp * e
Integral Term     : Ki * error_integral
Differential Term : Kd * de

`de = delta(e) / dt`
this means de is the slope

so... (de * dt) is simply delta(e)
that is why we have
`error_integral.at<double>(i, 0) += de.at<double>(i, 0) * dt;`

## Theory
