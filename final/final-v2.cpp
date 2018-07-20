#include <iostream>
#include <opencv2/opencv.hpp>
#include "pid.hpp"
#include "ardrone/ardrone.h"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;
using namespace cv::ml;

// WiFi AP: 118598

void detectAndDraw(Mat& image, CascadeClassifier& cascade, double scale);
cv::Mat getImage(VideoCapture &cap);
cv::Mat getImage(ARDrone &ardrone);  
void droneInit(ARDrone &ardrone);
void cascadeLoader(CascadeClassifier &face_cascade);
int existsMarkerID(vector<int> ids, int id);
int indexMarkerID(vector<int> ids, int id);
int changeStage(int& stage, int value);
bool rvecsCorrection(int id, double offset, int& enablePID, vector<int>& ids, vector<cv::Vec3d>& rvecs, Mat& input_error);
//left right correction
bool tvecs0Correction(int id, double base, double offset, int& enablePID, vector<int>& ids, vector<cv::Vec3d>& tvecs, Mat& input_error);
//distance correction
bool tvecs1Correction(int id, double base, double offset, int& enablePID,  vector<int>& ids, vector<cv::Vec3d>& tvecs, Mat& input_error);

int main( int argc, const char** argv )
{
    //ARDrone initialize
    ARDrone ardrone;
    //droneInit(ardrone);
    PIDManager PID("pid.yaml");
    PID.reset();

    //CascadeClassifier initialize
    CascadeClassifier face_cascade;
    cascadeLoader(face_cascade);

    //To Delete if drone is used
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        return -1;
    }

    Mat frame = getImage(cap);
     // Open XML file
    std::string filename("camera.xml");
    cv::FileStorage fs(filename, cv::FileStorage::READ);

    // Load camera parameters
    cv::Mat cameraMatrix, distCoeffs;
    fs["intrinsic"] >> cameraMatrix;
    fs["distortion"] >> distCoeffs;

    // Create undistort map
    cv::Mat mapx, mapy;
    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cameraMatrix, frame.size(), CV_32FC1, mapx, mapy);

    // Generate marker dictionary
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

    // Settings
    double markerLength = 0.094;
    int stage;
    changeStage(stage, 0); // before takeoff

    while(1){
                // Key input
        int key = cv::waitKey(33);
        if (key == 0x1b) break;

        // Take off / Landing 
        if (key == ' ') {
            if (ardrone.onGround()) ardrone.takeoff();
            else                    ardrone.landing();
        }

        // Move
        double vx = 0.0, vy = 0.0, vz = 0.0, vr = 0.0;
        if (key == 'i' || key == CV_VK_UP)    vx =  1.0;
        if (key == 'k' || key == CV_VK_DOWN)  vx = -1.0;
        if (key == 'u' || key == CV_VK_LEFT)  vr =  1.0;
        if (key == 'o' || key == CV_VK_RIGHT) vr = -1.0;
        if (key == 'j') vy =  1.0;
        if (key == 'l') vy = -1.0;
        if (key == 'q') vz =  1.0;
        if (key == 'a') vz = -1.0;
        ardrone.move3D(vx, vy, vz, vr);

        // Change camera
        static int mode = 0;
        if (key == 'c') ardrone.setCamera(++mode % 4);

        Mat image_raw = getImage(cap);
        Mat image;

        //No key pressed
        if(key == -1){

            // no key pressed
            cv::remap(image_raw, image, mapx, mapy, cv::INTER_LINEAR);

            std::vector<int> ids;
            std::vector< std::vector<cv::Point2f> > corners;
            cv::aruco::detectMarkers(image, dictionary, corners, ids);
            std::vector<cv::Vec3d> rvecs, tvecs;

            // used for PID management
            Mat input_error = Mat::zeros(4, 1, CV_64F);
            Mat output = Mat::zeros(4, 1, CV_64F);

            if (ids.size() > 0)
            { // marker(s) detected
                cv::aruco::estimatePoseSingleMarkers(corners, markerLength, cameraMatrix, distCoeffs, rvecs, tvecs);
                for(int i = 0; i < ids.size(); i++)
                    cv::aruco::drawAxis(image, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);

                    std::ostringstream stream;
                    stream << tvecs[0];
                    cv::putText(image, stream.str(), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
            }
            else
            { // no marker
            }

            int enablePID = 0;
            int index = -1;
            int pass;

            
            switch(stage){
                case 10:
                    
                    if( existsMarkerID(ids, 1))     changeStage(stage, 11);
                    else 
                        vr = 0.3;
                    break;

                case 11:
                    // check for marker & REVERT condition
                    if( rvecsCorrection(1, 0.3, enablePID, ids, rvecs, input_error))        changeStage(stage, 12);

                    else if (!enablePID)
                        vr = 0.2;
                    break;

                case 12:
                    // check for marker & REVERT condition
                    if( tvecs0Correction(1, 0.6, 0.1, enablePID, ids, tvecs, input_error))  changeStage(stage, 20);
                    break;
            }
            
        }

        detectAndDraw(image, face_cascade, 1.5);

        ardrone.move3D(vx, vy, vz, vr);
    }

    // See you
    ardrone.close();
    return 0;
}

void detectAndDraw(Mat& image, CascadeClassifier& cascade, double scale)
{
  const static Scalar colors[] =
      {
          Scalar(255,0,0),
          Scalar(255,128,0),
          Scalar(255,255,0),
          Scalar(0,255,0),
          Scalar(0,128,255),
          Scalar(0,255,255),
          Scalar(0,0,255),
          Scalar(255,0,255)
      };
  Mat gray, smallImg;
  vector< Rect > faces;

  cvtColor(image, gray, COLOR_BGR2GRAY);
  double fx = 1/scale;
  resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT );
  equalizeHist( smallImg, smallImg );

  // DETECT FACES
  cascade.detectMultiScale(smallImg, faces, scale, 2, 0 | CASCADE_SCALE_IMAGE, Size(30,30));

  // DRAW RECTANGLE
  for(int i = 0; i < faces.size(); i++)
  {
    Rect r = faces[i];
    Scalar color = colors[i%8];
    rectangle( image, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
                       cvPoint(cvRound((r.x + r.width-1)*scale), cvRound((r.y + r.height-1)*scale)),
                       color, 3, 8, 0);

  }

  // DISPLAY OUTPUT
  resize(image, image, Size(), 0.5, 0.5, INTER_LINEAR);

  imshow("image", image);

  int largestIdx = 0;
  for(int i = 0; i < faces.size(); i++){
    if(faces[largestIdx].width < faces[i].width)
        largestIdx = i;
  }

  if(faces.size() > 0 && faces[largestIdx].width > 150)
    cout<<"Too close"<<endl;

  if(faces.size() > 0)
    cout << faces[largestIdx].width << endl;
  // resizeWindow("image", image.cols*0.5, image.rows*0.5);

}

cv::Mat getImage(ARDrone &ardrone){
    // Get an image
    cv::Mat image = ardrone.getImage();
    return image;
}

cv::Mat getImage(VideoCapture &cap){
     // Get an image
    cv::Mat frame;
    cap >> frame;
    return frame;
}

void droneInit(ARDrone &ardrone){

    // Initialize
    if (!ardrone.open()) {
        std::cout << "Failed to initialize." << std::endl;
        exit(-1);
    }

     // Battery
    std::cout << "Battery = " << ardrone.getBatteryPercentage() << "[%]" << std::endl;

    // Instructions
    std::cout << "***************************************" << std::endl;
    std::cout << "*       CV Drone sample program       *" << std::endl;
    std::cout << "*           - How to play -           *" << std::endl;
    std::cout << "***************************************" << std::endl;
    std::cout << "*                                     *" << std::endl;
    std::cout << "* - Controls -                        *" << std::endl;
    std::cout << "*    'Space' -- Takeoff/Landing       *" << std::endl;
    std::cout << "*    'Up'    -- Move forward          *" << std::endl;
    std::cout << "*    'Down'  -- Move backward         *" << std::endl;
    std::cout << "*    'Left'  -- Turn left             *" << std::endl;
    std::cout << "*    'Right' -- Turn right            *" << std::endl;
    std::cout << "*    'Q'     -- Move upward           *" << std::endl;
    std::cout << "*    'A'     -- Move downward         *" << std::endl;
    std::cout << "*                                     *" << std::endl;
    std::cout << "* - Others -                          *" << std::endl;
    std::cout << "*    'C'     -- Change camera         *" << std::endl;
    std::cout << "*    'Esc'   -- Exit                  *" << std::endl;
    std::cout << "*                                     *" << std::endl;
    std::cout << "***************************************" << std::endl;
}

void cascadeLoader(CascadeClassifier &face_cascade){
    //face_cascade.load("haarcascade_frontalface_default.xml");
    //face_cascade.load("haarcascade_frontalface_alt2.xml");
    //face_cascade.load("haarcascade_frontalface_alt.xml");
  
    if( !face_cascade.load("haarcascade_frontalface_alt.xml")){
        cout << "error: loading cascade" << endl;
        exit(-1);
    }
}

int existsMarkerID(vector<int> ids, int id)
{ // 0: false, 1: true
    for(int i = 0; i < ids.size(); i++){
        if(id == ids[i])
            return 1;
    }
    return 0;
}

int indexMarkerID(vector<int> ids, int id){
    for(int i = 0; i < ids.size(); i++){
        if(id == ids[i])
            return i;
    }
    return -1;
}

int changeStage(int& stage, int value)
{
    stage = value;
    cout << "stage " << stage << endl;
    return 0;
}

bool rvecsCorrection(int id, double offset, int& enablePID, vector<int>& ids, vector<cv::Vec3d>& rvecs, Mat& input_error){
     int index = -1;
     if( existsMarkerID(ids, id) ){
            index = indexMarkerID(ids, id);
            cout << "rvec: " << rvecs[index][0] << " " << rvecs[index][1] << " " << rvecs[index][2] << endl;

            if( rvecs[index][2] > -offset && rvecs[index][2] < offset){
                return true;
            } 
            else {
                enablePID = 1;
                input_error.at<double>(3) = rvecs[index][2];
            }
      }
      return false;
}

//left right correction
bool tvecs0Correction(int id, double base, double offset, int& enablePID, vector<int>& ids, vector<cv::Vec3d>& tvecs, Mat& input_error){
    int index = -1;
    if( existsMarkerID(ids, id)){
        index = indexMarkerID(ids, id);
        if(tvecs[index][0] > base - offset && tvecs[index][0] < base + offset){
            return true;
        } 
        else {
            enablePID = 1;
            input_error.at<double>(1) = base - tvecs[index][0]; 
        }
    }
    return false;
}

//distance correction
bool tvecs1Correction(int id, double base, double offset, int& enablePID,  vector<int>& ids, vector<cv::Vec3d>& tvecs, Mat& input_error){
    int index = -1;
    if( existsMarkerID(ids, id)){
            index = indexMarkerID(ids, id);
            if(tvecs[index][2] > base - offset && tvecs[index][2] < base + offset){
                return true;
            } 
            else {
                enablePID = 1;
                input_error.at<double>(0) = tvecs[index][2] - base;
        }
    }
    return false;
}