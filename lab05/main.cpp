#include "ardrone/ardrone.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/aruco.hpp>

// Parameter for calibration pattern
#define PAT_ROWS   (6)                  // Rows of pattern
#define PAT_COLS   (9)                 // Columns of pattern
#define CHESS_SIZE (22.0)               // Size of a pattern [mm]

// --------------------------------------------------------------------------
// main(Number of arguments, Argument values)
// Description  : This is the entry point of the program.
// Return value : SUCCESS:0  ERROR:-1
// --------------------------------------------------------------------------
using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    // AR.Drone class
    ARDrone ardrone;

    // Initialize
    if (!ardrone.open()) {
        std::cout << "Failed to initialize." << std::endl;
        return -1;
    }

    // Images
    cv::Mat frame = ardrone.getImage();

    // Open XML file
    std::string filename("camera.xml");
    cv::FileStorage fs(filename, cv::FileStorage::READ);

    // Not found
    if (!fs.isOpened()) {
        // Image buffer
        std::vector<cv::Mat> images;
        std::cout << "Press Space key to capture an image" << std::endl;
        std::cout << "Press Esc to exit" << std::endl;

        // Calibration loop
        while (1) {
            // Key iput
            int key = cv::waitKey(33);
            if (key == 0x1b) break;

            // Get an image
            frame = ardrone.getImage();

            // Convert to grayscale
            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

            // Detect a chessboard
            cv::Size size(PAT_COLS, PAT_ROWS);
            std::vector<cv::Point2f> corners;
            bool found = cv::findChessboardCorners(gray, size, corners, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK);

            // Chessboard detected
            if (found) {
                // Draw it
                // cout << "found" << endl;
                cv::drawChessboardCorners(frame, size, corners, found);

                // Space key was pressed
                if (key == ' ') {
                    // Add to buffer
                    images.push_back(gray);
                }
            }

            // Show the image
            std::ostringstream stream;
            stream << "Captured " << images.size() << " image(s).";
            cv::putText(frame, stream.str(), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
            cv::imshow("Camera Calibration", frame);
        }

        // We have enough samples
        if (images.size() > 4) {
            cv::Size size(PAT_COLS, PAT_ROWS);
            std::vector< std::vector<cv::Point2f> > corners2D;
            std::vector< std::vector<cv::Point3f> > corners3D;

            for (size_t i = 0; i < images.size(); i++) {
                // Detect a chessboard
                std::vector<cv::Point2f> tmp_corners2D;
                bool found = cv::findChessboardCorners(images[i], size, tmp_corners2D);

                // Chessboard detected
                if (found) {
                    // Convert the corners to sub-pixel
                    cv::cornerSubPix(images[i], tmp_corners2D, cvSize(11, 11), cvSize(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 30, 0.1));
                    corners2D.push_back(tmp_corners2D);

                    // Set the 3D position of patterns
                    const float squareSize = CHESS_SIZE;
                    std::vector<cv::Point3f> tmp_corners3D;
                    for (int j = 0; j < size.height; j++) {
                        for (int k = 0; k < size.width; k++) {
                            tmp_corners3D.push_back(cv::Point3f((float)(k*squareSize), (float)(j*squareSize), 0.0));
                        }
                    }
                    corners3D.push_back(tmp_corners3D);
                }
            }

            // Estimate camera parameters
            cv::Mat cameraMatrix, distCoeffs;
            std::vector<cv::Mat> rvec, tvec;
            cv::calibrateCamera(corners3D, corners2D, images[0].size(), cameraMatrix, distCoeffs, rvec, tvec);
            std::cout << cameraMatrix << std::endl;
            std::cout << distCoeffs << std::endl;

            // Save them
            cv::FileStorage tmp(filename, cv::FileStorage::WRITE);
            tmp << "intrinsic" << cameraMatrix;
            tmp << "distortion" << distCoeffs;
            tmp.release();

            // Reload
            fs.open(filename, cv::FileStorage::READ);
        }

        // Destroy windows
        cv::destroyAllWindows();
    }

    // Load camera parameters
    cv::Mat cameraMatrix, distCoeffs;
    fs["intrinsic"] >> cameraMatrix;
    fs["distortion"] >> distCoeffs;

    // Create undistort map
    cv::Mat mapx, mapy;
    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cameraMatrix, frame.size(), CV_32FC1, mapx, mapy);

    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    double markerLength = 0.07;

    // Main loop
    while (1) {
        // Key input
        int key = cv::waitKey(33);
        if (key == 0x1b) break;

        // Get an image
        //cv::Mat image_raw = GP.getImage();
       //Mat image_raw = cap.read();
	      Mat image_raw;
	      image_raw = ardrone.getImage();

        // Undistort
        cv::Mat image;
        cv::remap(image_raw, image, mapx, mapy, cv::INTER_LINEAR);

        // Display the image
        cv::imshow("camera", image);

        std::vector<int> ids;
        std::vector< std::vector<cv::Point2f> > corners;

        cv::aruco::detectMarkers(image, dictionary, corners, ids);

        if(ids.size() > 0){
          // cout << "marker found" << endl;
          std::vector<cv::Vec3d> rvecs, tvecs;
          cv::aruco::estimatePoseSingleMarkers(corners, markerLength, cameraMatrix, distCoeffs, rvecs, tvecs);
          cv::aruco::drawAxis(image, cameraMatrix, distCoeffs, rvecs[0], tvecs[0], 0.1);
          // cv::imshow("camera", image);

          // if(key == ' ')
          //   cout << tvecs[0] << endl;

          // Show the image
          std::ostringstream stream;
          stream << tvecs[0];
          cv::putText(image, stream.str(), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
          // cv::imshow("Camera Calibration", frame);
          cv::imshow("camera", image);

        } else {
          // cout << "no marker" << endl;
        }

    }

    // See you
    ardrone.close();

    return 0;
}
