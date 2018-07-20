#include <cstdio>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

// 1. Read the image
// 2. Load face cascade
// 3. Detect faces
// 4. Draw rectangles on the detected faces
// 5. Show the result
void detectAndDraw(Mat& image, CascadeClassifier& cascade,
              CascadeClassifier& nestedCascade, double scale);

int main(int argc, char **argv){

  if(argc < 2){
    cout << "./lab08 <image_name>" << endl;
    return -1;
  }

  Mat image;
  CascadeClassifier cascade, nestedCascade;
  string cascadeLocation = "/usr/local/Cellar/opencv/3.4.1_2/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
  string nestedCascadeLocation = "/usr/local/Cellar/opencv/3.4.1_2/share/OpenCV/haarcascades/haarcascade_eye_tree_eyeglasses.xml";

  // LOAD CASCADES
  if( !cascade.load(cascadeLocation) ){
    cout << "error: loading cascade" << endl;
    return -1;
  }

  if( !nestedCascade.load(nestedCascadeLocation) ){
    cout << "error: loading nested cascade" << endl;
    return -1;
  }

  // LOAD IMAGE
  image = imread(argv[1], IMREAD_COLOR);
  if( !image.data ){
    cout << "error: reading image" << endl;
    return -1;
  }

  detectAndDraw(image, cascade, nestedCascade, 0.5);

  return 0;
}

void detectAndDraw(Mat& image, CascadeClassifier& cascade,
            CascadeClassifier& nestedCascade, double scale)
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
  cascade.detectMultiScale(smallImg, faces,
                      1.1, 2, 0
                      | CASCADE_SCALE_IMAGE, Size(30,30));

  // DRAW RECTANGLE
  for(int i = 0; i < faces.size(); i++)
  {
    Rect r = faces[i];
    Scalar color = colors[i%8];
    rectangle( image, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
                       cvPoint(cvRound((r.x + r.width-1)*scale), cvRound((r.y + r.height-1)*scale)),
                       color, 3, 8, 0);

    // NESTED DETECTION (CIRCLE)
    // Mat subImage = gray(r);
    // vector< Rect > nestedFaces;
    // Point center;
    // int radius;
    // nestedCascade.detectMultiScale(subImage, nestedFaces,
    //                             1.1, 2, 0
    //                             | CASCADE_SCALE_IMAGE, Size(30,30));
    //
    // for(int j = 0; j < nestedFaces.size(); j++)
    // {
    //   Rect nr = nestedFaces[j];
    //   center.x = cvRound((r.x + nr.x + nr.width*0.5)*scale);
    //         center.y = cvRound((r.y + nr.y + nr.height*0.5)*scale);
    //         radius = cvRound((nr.width + nr.height)*0.25*scale);
    //         circle( image, center, radius, color, 3, 8, 0 );
    // }
  }

  // DISPLAY OUTPUT

  resize(image, image, Size(), 0.5, 0.5, INTER_LINEAR);
  // namedWindow("image", 0);
  imshow("image", image);
  cout << faces.size() << endl;
  // resizeWindow("image", image.cols*0.5, image.rows*0.5);

  waitKey(0);
}
