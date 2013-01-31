#include "opencv/cv.h"
#include "opencv/highgui.h"

#ifdef OPENCV_2_1
#  include "opencv/highgui.hpp"
#else
#  include "opencv2/highgui/highgui.hpp"
#endif

#include <iostream>
#include <fstream>
#include <string>
using namespace cv;

#define USAGE "USAGE: find-face --cascade=PATH.xml [ --scale=SCALE ] IMAGE"

int main(int argc, const char** argv)
{
  std::string cascade_name;
  std::string input_name;
  CascadeClassifier cascade;
  double scale = 1;

  const std::string help_opt = "--help";
  const std::string scale_opt = "--scale=";
  const std::string cascade_opt = "--cascade=";

  for (int i = 1; i < argc; i++) {
    if (cascade_opt.compare(0, cascade_opt.length(), argv[i], cascade_opt.length()) == 0) {
      cascade_name.assign(argv[i] + cascade_opt.length());
    } else if (scale_opt.compare(0, scale_opt.length(), argv[i], scale_opt.length()) == 0) {
      std::istringstream arg(argv[i] + scale_opt.length());
      double scale;
      arg >> scale;
      if (scale < 1)
        scale = 1;
      std::cout << scale << std::endl;
      return 0;
    } else if (help_opt == argv[i]) {
      std::cout << USAGE << std::endl;
      return 0;
    } else {
      input_name.assign(argv[i]);
    }
  }

  if (cascade_name.empty()) {
    std::cerr << "ERROR: --cascade=PATH argument required" << std::endl;
    return -1;
  }

  if (!cascade.load(cascade_name)) {
    std::cerr << "ERROR: could not load classifier cascade: " << cascade_name << std::endl;
    return -1;
  }

  if (input_name.empty()) {
    std::cerr << "ERROR: image name required" << std::endl;
    return -1;
  }

  std::ifstream file(input_name.c_str());
  if (!file.good()) {
    std::cerr << "file not found or not readable: " << input_name << std::endl;
    return -1;
  }

  Mat image = imread(input_name, 1);
  if (!image.empty()) {
    Mat gray, small_image(cvRound(image.rows/scale), cvRound(image.cols/scale), CV_8UC1);
    cvtColor(image, gray, CV_BGR2GRAY);
    resize(gray, small_image, small_image.size(), 0, 0, INTER_LINEAR);
    equalizeHist(small_image, small_image);

    vector<Rect> faces;
    cascade.detectMultiScale(small_image, faces, 1.1, 2, 0, Size(30, 30));
    if (!faces.empty()) {
      unsigned int max_area = 0;
      vector<Rect>::const_iterator max_region = faces.end();
      for (vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++) {
        float area = r->width * r->height;
        if (area > max_area) {
          max_area = area;
          max_region = r;
        }
      }

      float image_width  = static_cast<float>(image.rows);
      float image_height = static_cast<float>(image.cols);

      /* These shouldn't happen, but guard for stability. */
      if (max_region == faces.end() || image_width == 0 || image_height == 0)
        return 0;

      std::cout
        << max_region->x / image_width << " "
        << max_region->y / image_height << " "
        << max_region->width / image_width << " "
        << max_region->height / image_height << std::endl;
    }
  }

  return 0;
}
