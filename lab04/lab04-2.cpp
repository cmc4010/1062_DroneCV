#include <cstdio>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

// write down your warping function here
void warpImage(Mat& input, Mat& output) {

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
	cap_corner.push_back(Point2f(0, frame.rows-1));
	cap_corner.push_back(Point2f(frame.cols-1, frame.rows-1));

	namedWindow("img", CV_WINDOW_AUTOSIZE);
	setMouseCallback("img", onMouse, &img_corner);

	while (img_corner.size()<4) {
		imshow("img", image);
		if (waitKey(1) == 27) break; // ESC
	}

	Mat img_out = image.clone();
	Mat img_temp = image.clone();
	Mat h = findHomography(cap_corner, img_corner);
	// call your warping function

	Point poly[4];
	for (int i = 0; i < img_corner.size(); i++) {
		poly[i] = img_corner[i];
	}

	while (1) {
		cap >> frame;
		// call your warping function
		fillConvexPoly(img_out, poly, 4, Scalar(0), CV_AA);
		img_out = img_out + img_temp;
		imshow("img", img_out);
		if (waitKey(1) == 27) break; // ESC
	}
	return 0;
}
