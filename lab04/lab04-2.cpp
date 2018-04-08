#include <cstdio>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

// write down your warping function here
void warpImage(Mat& original, Mat& target, Mat& homography) {
	// for each pixel in original
		// perform homography
		// change the value of target to value of original

	Mat target_coord;
	for(int r = 0; r < original.rows; r++){
		for(int c = 0; c < original.cols; c++){
			// x: column, y: row
			double mydata[3];
			mydata[0] = c;
			mydata[1] = r;
			mydata[2] = 1;
			Mat mymat(3,1,CV_64F,mydata); // (row, col, type, )
			target_coord = homography * mymat;

			// (row, col)
			double x = target_coord.at<double>(0,0);
			double y = target_coord.at<double>(1,0);
			double z = target_coord.at<double>(2,0);

			// divide x and y by z
			x /= z; y /= z;

			if( (int) x < target.cols && (int) y < target.rows){
				target.at<Vec3b>((int)y, (int)x) = original.at<Vec3b>(r,c);
			}
		}
	}

}

void onMouse(int event, int x, int y, int flags, void* param) {
	vector<Point2f>* ptr = (vector<Point2f>*) param;
	if (event == CV_EVENT_LBUTTONDOWN) {
		ptr->push_back(Point2f(x, y));
		cout << "x: " << x << endl;
		cout << "y: " << y << endl;
	}
}

int main(int argc, char **argv) {

	Mat image;
	if(argc > 1)
		image = imread(argv[1]);
	else
		return -1;

	VideoCapture cap(0);
	if (!cap.isOpened()) {
		return -1;
	}

	Mat frame;
	cap >> frame;

	vector<Point2f> cap_corner; // webcam frame corners
	vector<Point2f> img_corner;

  // add the corner of frame into cap_corner
	// x is column, y is row
	cap_corner.push_back(Point2f(0, 0));
	cap_corner.push_back(Point2f(frame.cols-1, 0));
	cap_corner.push_back(Point2f(frame.cols-1, frame.rows-1));
	cap_corner.push_back(Point2f(0, frame.rows-1));

	namedWindow("img", CV_WINDOW_AUTOSIZE);
	setMouseCallback("img", onMouse, &img_corner);

	while (img_corner.size()<4) {
		imshow("img", image);
		if (waitKey(1) == 27) break; // ESC
	}

	Mat img_out = image.clone();
	Mat img_temp = image.clone();
	Mat h = findHomography(cap_corner, img_corner); // (original, target)

	// CLEAR img_temp
	Point temp_poly[4]; // (col, row)
	temp_poly[0] = Point2f(0, 0);
	temp_poly[1] = Point2f(img_temp.cols-1, 0);
	temp_poly[2] = Point2f(img_temp.cols-1, img_temp.rows-1);
	temp_poly[3] = Point2f(0, img_temp.rows-1);
	fillConvexPoly(img_temp, temp_poly, 4, Scalar(0), CV_AA);

	Point poly[4];
	for (int i = 0; i < img_corner.size(); i++) {
		poly[i] = img_corner[i];
	}

	while (1) {
		cap >> frame;
		warpImage(frame, img_temp, h);
		fillConvexPoly(img_out, poly, 4, Scalar(0), CV_AA); // empty out
		img_out = img_out + img_temp;
		imshow("img", img_out);
		if (waitKey(1) == 27) break; // ESC
	}
	return 0;
}
