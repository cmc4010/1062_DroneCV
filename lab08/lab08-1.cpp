#include <cstdio>
#include <opencv2/opencv.hpp>
#include "opencv2/core/mat.hpp"  
#include <iostream>

using namespace std;
using namespace cv;

int main( int argc, const char** argv )
{
    string video_location = "pedestrian.mp4";
    /// Create a videoreader interface
    VideoCapture cap(video_location);
    Mat current_frame;

    /// Set up the pedestrian detector --> let us take the default one
    HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    /// Set up tracking vector
    vector<Point> track;

    while(true){
        /// Grab a single frame from the video
        cap >> current_frame;
        if(current_frame.empty()){
            cerr << "Video has ended or bad frame was read. Quitting." << endl;
            return 0;
        }

        Mat img = current_frame.clone();
        resize(img,img,Size(img.cols, img.rows));

        vector<Rect> found;
        vector<double> weights;

        hog.detectMultiScale(img, found, weights, 0, Size(7, 7), Size(8,8), 1.03);

        /// draw detections and store location
        for( size_t i = 0; i < found.size(); i++ )
        {
            Rect r = found[i];
            rectangle(img, found[i], cv::Scalar(0,0,255), 3);
		}

        /// Show
        imshow("detected person", img);
        waitKey(1);
    }

    return 0;
}