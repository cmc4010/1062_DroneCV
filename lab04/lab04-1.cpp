#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <cmath>

using namespace std;
using namespace cv;

int main(int argc, char ** argv){

  VideoCapture cap(1); // default device

  Mat original, gray;
  bool found;
  vector< vector<Point2f> > corners;
  vector< vector<Point3f> > points3D(10);
  Mat intrinsic, distortionCoeffs;
  vector<Mat> Rvecs, Tvecs;

  for(int t = 0; t < 10; t++){
    for(int i = 0; i < 6; i++){
      for(int j = 0; j < 9 ; j++){
        points3D[t].push_back(Point3f(i, j, 0.0f));
      }
    }
  }

  int count = 0;
  while(count < 10){
    cap >> original;
    cvtColor(original, gray, CV_BGR2GRAY);

    vector<Point2f> local_corners;

    found = findChessboardCorners(gray, Size(9, 6), local_corners);

    if(found){
      // cout << "board found" << endl;
      cornerSubPix(gray, local_corners, Size(11, 11), Size(-1,-1),
        TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 30, 0.1) );
      drawChessboardCorners(original, Size(9, 6), Mat(local_corners), found);
    }

    imshow("webcam", original);

    if(waitKey(1) == 32 && found){
      corners.push_back(local_corners);
      cout << "# " << count << endl;
      count++;
    }

  }

  double calib_result = calibrateCamera(points3D, corners, original.size(),
    intrinsic, distortionCoeffs, Rvecs, Tvecs);

  cout << "Camera calibrated " << calib_result << endl;

  Mat outputMapX, outputMapY;
  outputMapX.create(original.size(), CV_32FC1);
  outputMapY.create(original.size(), CV_32FC1);

  initUndistortRectifyMap(  intrinsic,
                            distortionCoeffs,  Mat(),
                            intrinsic,  original.size(),  CV_32FC1,
                            outputMapX,
                            outputMapY  );

  // Output Data
  FileStorage fs("calibration4.xml", FileStorage::WRITE);
  fs << "intrinsic" << intrinsic;
  fs << "distortion" << distortionCoeffs;

  Mat outputImage;

  while(1){
    cap >> original;

    remap(  original,
            outputImage,
            outputMapX,
            outputMapY,
            INTER_LINEAR );

    imshow("webcam", original);
    imshow("calibrated webcam", outputImage);

    if( waitKey(33) == 13){
      break;
    }
  }

  return 0;
}
