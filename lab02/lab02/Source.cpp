#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>

using namespace std;
using namespace cv;

void applyHistogramEqualization(Mat& input, Mat& output);
void applyLaplacianFilter(Mat& input, Mat& output);

int 	main(int argc, char** argv) {

	Mat inputImg = imread("mj.tif", 0); // read as grey scale
	
	// clone inputImg to get the same image size
	Mat outputImg1 = inputImg.clone();
	Mat outputImg2 = inputImg.clone();

	imshow("Input", inputImg);

	applyHistogramEqualization(inputImg, outputImg1);
	applyLaplacianFilter(inputImg, outputImg2);

	imshow("HistogramEqualization", outputImg1);
	imshow("Laplacian Filter", outputImg2);

	waitKey(0); // pause

	return 0;
}

void applyHistogramEqualization(Mat& input, Mat& output){

	/* PSEUDOCODE: Basic Histogram h(k) */
	// initialize an array to all 0s
	// for every point
		// increase the count of the grey level that the point possess

	int histogram[256] = { 0 }; // initialize to all 0s

	for (int r = 0; r < input.rows; r++){
		for (int c = 0; c < input.cols; c++){
			// color value of every point will be in [0,255]
			histogram[input.at<uchar>(r, c)]++;
		}
	}
	
	/* PSEUDOCODE: Compute Transformation Function T(k) */
	// create array to store transformed values
	// for each grey level k
		// sum up values from h(0) up to h(k)
		// divide the summation by the # of pixels in image
		// multiply final value by L-1 which in this case is 255
		// save final value in array

	int transformed[256] = { 0 };
	float total;

	for (int i = 0; i < 256; i++){
		total = 0;
		for (int k = 0; k <= i; k++){
			total += histogram[k];
		}
		total /= input.rows * input.cols; // # of pixels
		total *= 255;
		transformed[i] = (int)total;
	}
	
	// create output image with transformation function T(k) values
	for (int r = 0; r < input.rows; r++){
		for (int c = 0; c < input.cols; c++){
			output.at<uchar>(r, c) = transformed[input.at<uchar>(r, c)];
		}
	}
}

void applyLaplacianFilter(Mat& input, Mat& output){

	// given 3x3 map
	//	0	1	0
	//	1	-4	1
	//	0	1	0
	
	int map[3][3];
	map[0][0] = map[0][2] = map[2][0] = map[2][2] = 0;
	map[1][0] = map[0][1] = map[1][2] = map[2][1] = 1;
	map[1][1] = -4;

	/* PSEUDOCODE: Laplacian Filter with 3x3 Map */
	// for each pixel
		// apply 3x3 filter with 0-fill for out-of-bounds
			// add up all pixels within 3x3 area with corresponding "weights" applied for each pixel
		// save value to output image

	// For every single pixel... apply MAP with 0-fill
	for (int r = 0; r < input.rows; r++){
		for (int c = 0; c < input.cols; c++){
			
			int total = 0;

			for (int i = 0; i < 3; i++){
				for (int j = 0; j < 3; j++){
					int ref_row = r - 1 + i;
					int ref_col = c - 1 + j;
					
					if (ref_row < 0 || ref_col < 0 || ref_row == input.rows || ref_col == input.cols){ // handles 0 fill
						total += 0;
					}
					else {
						total += input.at<uchar>(ref_row, ref_col) * map[i][j];
					}
				}
			}

			// save into output
			if (total < 0){
				total = 0;
			}
			else if (total > 255){
				total = 255;
			}

			output.at<uchar>(r, c) = total;
		}
	}
}