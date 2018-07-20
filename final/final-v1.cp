#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace cv::ml;

void detectAndDraw(Mat& image, CascadeClassifier& cascade, double scale);

int main( int argc, const char** argv )
{
		CascadeClassifier face_cascade;
		//face_cascade.load("haarcascade_frontalface_default.xml");
		//face_cascade.load("haarcascade_frontalface_alt2.xml");

		string cascadeLocation = "/usr/local/Cellar/opencv/3.4.1_2/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";

	  if( !face_cascade.load(cascadeLocation)){
	    	cout << "error: loading cascade" << endl;
	    	return -1;
	 	}

	  VideoCapture cap(0);
	  if (!cap.isOpened()) {
	      return -1;
	  }

	  while(1){
	    Mat frame;
	    cap >> frame;

			Mat input_img = frame.clone();
			// Mat output_img = input_img.clone();

		 	detectAndDraw(input_img, face_cascade, 1.5);
			if(waitKey(1) == 27) break;
	  }

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
  for(int i = 0; i < faces.size(); i++)
	{
  	if(faces[largestIdx].width < faces[i].width)
  		largestIdx = i;
  }

	if(faces.size() > 0)
		 cout << faces[largestIdx].width << endl;

  if(faces.size() > 0 && faces[largestIdx].width > 260) // 101， *152， 228
  	cout<<"Too close"<<endl;

  // cout << faces.size() << endl;
}
