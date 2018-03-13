#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>

using namespace std;
using namespace cv;

void 	bilinear_interpolation(Mat& input, Mat& output, float scalingFactor);
void	bgr2rgb(Mat& input, Mat& output);

int 	main(int argc, char** argv) {

	Mat inputImg = imread("IU_small.jpg");
	float scalingFactor = 3.0;

	int scaledWidth = round(1. * inputImg.cols * scalingFactor);
	int scaledHeight = round(1. * inputImg.rows * scalingFactor);

	Mat outputImg1 = Mat(scaledHeight, scaledWidth, inputImg.type());
	Mat outputImg2; // for opencv built-in function
	Mat outputImg3 = inputImg.clone(); // for red-blue channel swap

	// swap red and blue channels
	bgr2rgb(inputImg, outputImg3);

	// custom bilinear_interpolation function
	bilinear_interpolation(inputImg, outputImg1, scalingFactor);

	// bilinear interpolation with built-in function
	resize(inputImg, outputImg2, Size(), scalingFactor, scalingFactor, INTER_LINEAR);

	imshow("My Interpolation", outputImg1);
	imshow("Opencv build-in function", outputImg2);
	imshow("Red-Blue Swap", outputImg3);
	waitKey(0); // pause

	/*
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	compression_params.push_back(100);

	try{
		imwrite("output.jpg", outputImg1, compression_params);
	}
	catch (runtime_error& ex){
		fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
		return 1;
	}

	fprintf(stdout, "Image output as JPG successfully.\n");
	waitKey(0); // pause
	*/

	return 0;
}


void 	bilinear_interpolation(Mat& input, Mat& output, float scalingFactor) {

	// PSEUDOCODE
	// for each point in output
		// find coordinate on original image
		// perform bilinear interpolation

	/* EXAMPLE */
	// Coordinate: (1.5, 6.2)
	//
	// reference point: (1,6)
	// all points: (1,6), (1,7), (2,6), (2,7)

	double scaled_row, scaled_col;
	double N, W, E, S;
	int ref_row, ref_col;
	double tl_comp, tr_comp, bl_comp, br_comp;

	for (int r = 0; r < output.rows; r++){
		for (int c = 0; c < output.cols; c++){

			// get the coordinate on original image
			scaled_row = r / scalingFactor;
			scaled_col = c / scalingFactor;

			// reference point
			ref_row = floor(scaled_row);
			ref_col = floor(scaled_col);

			N = scaled_row - ref_row;
			W = scaled_col - ref_col;
			E = 1 - W;
			S = 1 - N;

			// computation
			for (int i = 0; i < 3; i++){

				// TOP-LEFT
				tl_comp = S*E*input.at<Vec3b>(ref_row, ref_col)[i];

				// TOP-RIGHT
				if (ref_col == input.cols - 1)
					tr_comp = W*S*input.at<Vec3b>(ref_row, ref_col)[i];
				else
					tr_comp = W*S*input.at<Vec3b>(ref_row, ref_col + 1)[i];

				// BOTTOM-LEFT
				if (ref_row == input.rows - 1)
					bl_comp = N*E*input.at<Vec3b>(ref_row, ref_col)[i];
				else
					bl_comp = N*E*input.at<Vec3b>(ref_row + 1, ref_col)[i];

				// BOTTOM-RIGHT
				if (ref_row == input.rows - 1 && ref_col == input.cols - 1)
					br_comp = N*W*input.at<Vec3b>(ref_row, ref_col)[i];
				else if (ref_row == input.rows - 1)
					br_comp = N*W*input.at<Vec3b>(ref_row, ref_col + 1)[i];
				else if (ref_col == input.cols - 1)
					br_comp = N*W*input.at<Vec3b>(ref_row + 1, ref_col)[i];
				else
					br_comp = N*W*input.at<Vec3b>(ref_row + 1, ref_col + 1)[i];

				output.at<Vec3b>(r, c)[i] = tl_comp + tr_comp + bl_comp + br_comp;
			}
		}
	}
}

void	bgr2rgb(Mat& input, Mat& output) {

	// Regular: w x h
	// OpenCV: row x col

	// width: img.cols
	// height: img.rows

	// PSEUDOCODE
	// for all points
		// swap r and b

	// 0: blue, 1: green, 2: red
	for (int r = 0; r < input.rows; r++){
		for (int c = 0; c < input.cols; c++){
			output.at<Vec3b>(r, c)[0] = input.at<Vec3b>(r, c)[2];
			output.at<Vec3b>(r, c)[1] = input.at<Vec3b>(r, c)[1];
			output.at<Vec3b>(r, c)[2] = input.at<Vec3b>(r, c)[0];
		}
	}

}
