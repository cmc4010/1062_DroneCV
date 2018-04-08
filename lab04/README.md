
# Camera Calibration and Warping

## Requirements
Webcam (external or built-in)

## Theory
Note: We are using the pinhole camera model.

### What is the principal point offset?

The coordinate system that we are using for an image has its origin at the top-left corner. Given a point in world coordinate space, we use the extrinsic matrix to translate it to a coordinate system with respect to the camera. This camera coordinate system has the origin (principal point) at the center. Principle point offset is used to translate from the camera coordinate system to the image plane coordinate system.

[x y z]T = R[X Y Z]T + t
x' = x/z
y' = y/z
u = fx * x' + cx
v = fy * y' + cy

**Intrinsic**
fx,fy   : focal lengths
cx,cy   : principle point offset

**Extrinsic**
R       : rotation matrix
t       : translation vector

(X,Y,Z) : point in world coord
(x',y') : the point on the image plane (camera coord)
(u,v)   : the point on the image plane (image coord)

### What is the purpose of calibration?
Real lenses usually have distortions, and calibration is used to correct these distortions.
