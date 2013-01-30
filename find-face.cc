#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <fstream>
#include <string>
using namespace cv;

#define USAGE "USAGE: find-face --cascade=PATH.xml [ --scale=SCALE ] IMAGE"

bool starts_with(std::string &string, const char *comparator) {
  size_t length = string.length();
  return string.compare(0, length, comparator, length) == 0;
}

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
      if (!sscanf(argv[i] + scale_opt.length(), "%lf", &scale) || scale < 1)
        scale = 1;
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
    for (vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++) {
      float width  = static_cast<float>(image.rows);
      float height = static_cast<float>(image.cols);
      std::cout << r->x / width << " " << r->y / height << " " << r->width / width << " " << r->height / height << std::endl;
    }
  }

  return 0;
}
