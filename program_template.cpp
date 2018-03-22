#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>

using namespace std;
using namespace cv;

int main(int argv, char ** argv[]){

  Mat inputImg = imread("xxx.jpg");

  imshow("My image", inputImg);

  waitKey(0);

  return 0;
}
