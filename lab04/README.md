
# Camera Calibration and Warping

## Requirements
Webcam (external or built-in)

## 4-1 Calibration
Note: We are using the pinhole camera model.

Given a point in world coordinate space, we use the extrinsic matrix to translate it to a coordinate system with respect to the camera. Then we use the intrinsic matrix to get the point on the image plane. In this camera coordinate system, the origin (principal point) of the image plane is usually at the center. However, this is not what we want because we are used to having the origin at the top-left corner. Principle point offset is used to translate from the camera coordinate system to the image plane coordinate system that we are used to.

[x y z]T = R[X Y Z]T + t\
x' = x/z\
y' = y/z\
u = fx * x' + cx\
v = fy * y' + cy

**Intrinsic**\
fx,fy   : focal lengths\
cx,cy   : principle point offset

**Extrinsic**\
R       : rotation matrix\
t       : translation vector

(X,Y,Z) : point in world coord\
(x',y') : the point on the image plane (camera coord)\
(u,v)   : the point on the image plane (image coord)

### What is the principal point offset?
Again, the coordinate system that we are used to using for an image has its origin at the top-left corner. In order to achieve this, we need to add the principal point offset to the result of the translation from world coordinate system to the camera coordinate system.

### Why are there so many coordinate systems? (WIP)
This applies in cases where the camera is perhaps moved around. For example, viewing an object from different angles.

### What is the purpose of calibration? (WIP)
Real lenses usually have distortions. Calibration's objective is to estimate the intrinsic and extrinsic parameters for the camera to prevent such distortions.

### How does calibration work? (WIP)
If we know where a point lies in the image as well as its respective world coordinate space coordinates, then we can solve for the parameters.

Maybe you can think of it like... what are the settings necessary to make the points appear properly (or as specified). We are looking for the "settings".

### Methods (WIP)
1. Take images of the grid pattern in different angles.
2. Provide the pattern details
3. Calibrate using the corners extracted from images and pattern details

## 4-2 Warping
Mapping one image plane to another.

Say we want to place image 1 into image 2:\
After we get the homography, we can apply the matrix onto each pixel of image 1 to get the respective pixel on image 2. The final step is to copy the original pixel values to the respective pixel on image 2.

### What do we need to get homography matrix?
- Source area in image 1 marked by 4 corners
- Destination area in image 2 marked by 4 corners
Note: Make sure the order of the points are the same.

### How to use?
The source area is preset to be the full frame of webcam. The destination area is to be selected during runtime through mouse clicking the target image. **NOTE:** The order of selecting corners is important. Please start from the top-left corner and continue clockwise.

After you've selected 4 corners, you should immediately see your webcam frame in the designated area.
