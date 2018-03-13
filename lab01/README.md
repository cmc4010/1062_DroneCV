# Lab 01
## Overview

A quick exercise to familiarize with digital image processing and OpenCV.

**Exercises Covered:**

1. Color channel swap
1. Bilinear interpolation

## Part I: Color channel swap (50%)

**Objective:** Swap the BLUE and RED channel values.

Image format: JPG

**Notes:**

For a regular colored image, a pixel has 3 channels, consisting of BLUE, GREEN, and RED. When we use OpenCV to manipulate images, note that the order will be BGR and not RGB.

This means that when we run the line:

`input.at<Vec3b>(row, col)[0]`

The value is for BLUE.

**Things Learned:**

* Visual Studio project setup with OpenCV
* Read image with OpenCV
* Manipulate pixels

## Part II: Bilinear interpolation (50%)

**Objective:** Resize an image using bilinear interpolation.

Image format: JPG

**Notes:**

We will scale both the width and height by a scaling factor.

Map the pixel on the new image to the pixel on the original image. Using the mapped pixel as reference, get the bilinear interpolation value using the 4 neighboring pixels.

The weights for bilinear interpolation is named North, West, East, and South in the code for easy identification.

  N
W   E
  S

T(pixel) = p1 * E * S + p2 * W * S + p3 * N * E + p4 * N * W

p1, p2, p3, and p4 represent the neighboring pixels top-left, top-right, bot-left, and bot-right respectively.
