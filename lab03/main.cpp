#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <vector>

using namespace std;
using namespace cv;

/* small functions */
int findRoot(int * array, int target);
int sortArray(int * intArray);
int * createHistogram(Mat& input);
void printHistogram(int * histogram);

/* main functions */
int otsu_threshold(Mat& input, int method);
void perform_thresholding(Mat& input, Mat& output, int threshold);
void connected_component_labeling(Mat& input, Mat& output);

int main(int argc, char ** argv){

  Mat inputImg = imread(argv[1]); // read as grey scale
  Mat otsu_img = inputImg.clone();
  Mat outputImg;

  imshow("Input image", inputImg);

  int threshold = otsu_threshold(inputImg, 1);
  cout << "threshold: " << threshold << endl;
  perform_thresholding(inputImg, otsu_img, threshold);
  imshow("Otsu image", otsu_img);

  outputImg = otsu_img.clone();
  connected_component_labeling(otsu_img, outputImg);
  imshow("Output Image", outputImg);

  waitKey(0);

  return 0;
}

int findRoot(int * array, int target){
  // INPUT: label
  // OUTPUT: smallest label in set (including self)
  int index = target;
  int value = -1;

  while(1)
  { // find the root
    value = array[index];
    if(value == 0) break;
    else index = value;
  }
  return index;
}

int sortArray(int * intArray, int length)
{ // increasing order, return to first nonzero index
  for(int i = 0; i < length - 1 ; i++)
  { // to ensure the swap is complete (handles worst case)
    for(int j = 0; j < length - 1 ; j++)
    { // swap current and next each iteration
      if( intArray[j] > intArray[j+1])
      { // perform swap
        int temp = intArray[j];
        intArray[j] = intArray[j+1];
        intArray[j+1] = temp;
      }
    }
  }

  // return first nonzero index
  int index = 0;
  while( index < length && intArray[index] == 0)
    index++;
  return index;
}

int * createHistogram(Mat& input){
  int * histo = new int[256];
  // initialize
  for(int i = 0; i < 256 ; i++){
    histo[i] = 0;
  }
  // calculate values
  for(int r = 0; r < input.rows; r++){
    for(int c = 0; c < input.cols; c++){
      int pixel_value = input.at<Vec3b>(r,c)[0];
      histo[pixel_value]++;
    }
  }
  return histo;
}

void printHistogram(int * histogram)
{
  for(int i = 0; i < 256; i++){
    cout << i << ": " << histogram[i] << endl;
  }
}

int otsu_threshold(Mat& input, int method)
{ // INPUT: image, OUTPUT: threshold
  float variances[256] = { 0 };

  int * histo = createHistogram(input);

  int num_pixel = input.rows * input.cols;
  int num_bg; // black
  int num_fg; // white
  float weight_bg, mean_bg, variance_bg;
  float weight_fg, mean_fg, variance_fg;
  for(int T = 0; T < 256 ; T++){
    // calculate components

    // WEIGHT
    num_bg = 0; num_fg = 0;
    for(int i = 0 ; i < 256 ; i++){
      if(i < T)
        num_bg += histo[i];
      else
        num_fg += histo[i];
    }

    weight_bg = 1.0 * num_bg / num_pixel;
    weight_fg = 1.0 * num_fg / num_pixel;

    // MEAN
    mean_bg = 0; mean_fg = 0;
    for(int i = 0; i < 256 ; i++){
      if(i < T)
        mean_bg += i * histo[i];
      else
        mean_fg += i * histo[i];
    }

    if(num_bg != 0)
      mean_bg /= num_bg;
    if(num_fg != 0)
      mean_fg /= num_fg;

    // VARIANCE
    variance_bg = 0; variance_fg = 0;
    for(int i = 0; i < 256 ; i++){
      float difference = 1.0 * i - ((i < T)? mean_bg : mean_fg);
      if(i < T)
        variance_bg += difference * difference * histo[i];
      else
        variance_fg += difference * difference * histo[i];
    }

    if(num_bg != 0)
      variance_bg /= num_bg;
    if(num_fg != 0)
      variance_fg /= num_fg;

    // Method #1: lowest within class variance
    if(method == 1){
      variances[T] = weight_bg * variance_bg + weight_fg * variance_fg;
    }
    // Method #2: maximum between class variance
    else {
      variances[T] = weight_bg * weight_fg * (mean_bg - mean_fg) * (mean_bg - mean_fg);
    }
  }

  // select threshold
  int best_threshold = 0; // initial index
  for(int i = 1; i < 256 ; i++)
  {
    if(method == 1)
    { // get min
      if(variances[i] < variances[best_threshold])
        best_threshold = i;
    }
    else
    { // get max
      if(variances[i] > variances[best_threshold])
        best_threshold = i;
    }
  }

  // cleanup
  delete [] histo;

  return best_threshold;
}

void perform_thresholding(Mat& input, Mat& output, int threshold)
{ // for each pixel... assign 0 or 255
  for(int r = 0; r < input.rows ; r++){
    for(int c = 0; c < input.cols ; c++){
      if(input.at<Vec3b>(r,c)[0] < threshold)
      {
        output.at<Vec3b>(r,c)[0] = 0;
        output.at<Vec3b>(r,c)[1] = 0;
        output.at<Vec3b>(r,c)[2] = 0;
      }
      else
      {
        output.at<Vec3b>(r,c)[0] = 255;
        output.at<Vec3b>(r,c)[1] = 255;
        output.at<Vec3b>(r,c)[2] = 255;
      }
    }
  }
}

void connected_component_labeling(Mat& input, Mat& output)
{ // Two pass algorithm

  // NOTE: label and color the WHITE parts (255-only)

  /* INITIALIZATION */
  int numPixels = input.rows * input.cols;
  int ** labelMap = new int*[input.rows];
  int * UP = new int[input.rows * input.cols + 1];

  // LABEL FOR EACH ARRAY
  for(int i = 0; i < input.rows ; i++){
    labelMap[i] = new int[input.cols];
    for(int j = 0; j < input.cols ; j++){
      labelMap[i][j] = 0;
    }
  }

  // RELATIONSHIP ARRAY
  for(int i = 0; i < input.rows * input.cols + 1 ; i++){
    UP[i] = 0;
  }

  /* FIRST PASS: labeling and constructing equivalence */
  cout << "FIRST PASS" << endl;
  int label = 1; // next in line

  for(int r = 0; r < input.rows ; r++){
    for(int c = 0; c < input.cols ; c++){

      if (input.at<Vec3b>(r,c)[0] == 0)
      { // black, area we DON'T want

      }
      else
      { // white, area we want
        /* GET NEIGHBORS (8-connectivity) */
        int neighbors[4] = { 0 }; // NW, N, NE, W
        int lengthArray = 4;

        if( r-1 >= 0 && c-1 >= 0 && input.at<Vec3b>(r-1, c-1)[0] == 255)
        { // check NORTH-WEST, foreground
          neighbors[0] = labelMap[r-1][c-1];
        }

        if ( r-1 >= 0 && input.at<Vec3b>(r-1, c)[0] == 255)
        { // check NORTH, foreground
          neighbors[1] = labelMap[r-1][c];
        }

        if( r-1 >= 0 && c+1 < input.cols && input.at<Vec3b>(r-1, c+1)[0] == 255)
        { // check NORTH-EAST, foreground
          neighbors[2] = labelMap[r-1][c+1];
        }

        if ( c-1 >= 0 && input.at<Vec3b>(r, c-1)[0] == 255)
        { // check WEST, foreground
          neighbors[3] = labelMap[r][c-1];
        }

        // get the smallest label of neighbors
        int index = sortArray(neighbors, lengthArray);

        if(index == lengthArray)
        { // no valid entries, create new label
          labelMap[r][c] = label++;
        }
        else
        { // save smallest, construct relationships

          // assign smallest label to current pixel
          labelMap[r][c] = neighbors[index];

          /* RELATION CONSTRUCTION */
          int roots[4] = { 0 };

          for(int i = 0; i < lengthArray; i++)
          { // get the roots for each label
            roots[i] = findRoot(UP, neighbors[i]);
          }

          // get smallest root
          index = sortArray(roots, lengthArray);

          for(int i = index+1; i < lengthArray; i++)
          { // construct relations, point to smallest root
            if(roots[i] != roots[index])
              UP[roots[i]] = roots[index];
          }
        }
      }
    }
  }

  cout << "SECOND PASS" << endl;
  /* SECOND PASS: perform label translation */
  for( int r = 0; r < input.rows ; r++){
    for( int c = 0; c < input.cols ; c++){
      // for each pixel, get the smallest label
      labelMap[r][c] = findRoot(UP, labelMap[r][c]);
    }
  }

  cout << "COLORING STAGE" << endl;
  // THIRD PASS: color
  int colors[label][3];

  for(int i = 1; i < label; i++)
  { // give random color for each label
    colors[i][0] = rand() %256;
    colors[i][1] = rand() %256;
    colors[i][2] = rand() %256;
  }

  for( int r = 0; r < input.rows ; r++){
    for(int c = 0; c < input.cols ; c++){
      int pixel_label = labelMap[r][c];
      if(pixel_label != 0){
        output.at<Vec3b>(r,c)[0] = colors[pixel_label][0];
        output.at<Vec3b>(r,c)[1] = colors[pixel_label][1];
        output.at<Vec3b>(r,c)[2] = colors[pixel_label][2];
      }
    }
  }

  // CLEAN UP
  for(int i = 0; i < input.rows ; i++){
    delete [] labelMap[i];
  }
  delete [] labelMap;
  delete [] UP;

  cout << "all done." << endl;

}
