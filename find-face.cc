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
#include <vector>

using namespace cv;

#define USAGE "USAGE: find-face --cascade=PATH.xml [ --scale=SCALE ] [ --magickcenter ] IMAGE"

// int face_area_compare(Rect *face1, Rect *face2) 
// {
//   return ((face1.width * face1->.height) < (face2.width * face2.height));
// }

int main(int argc, const char** argv)
{
  std::string cascade_name;
  std::string input_name;
  CascadeClassifier cascade;
  double scale = 1;
  bool do_magick_center=false;

  const std::string help_opt = "--help";
  const std::string scale_opt = "--scale=";
  const std::string cascade_opt = "--cascade=";
  const std::string magickcenter = "--magickcenter";

  for (int i = 1; i < argc; i++) {
    if (cascade_opt.compare(0, cascade_opt.length(), argv[i], cascade_opt.length()) == 0) {
      cascade_name.assign(argv[i] + cascade_opt.length());
    } else if (scale_opt.compare(0, scale_opt.length(), argv[i], scale_opt.length()) == 0) {
      std::istringstream arg(argv[i] + scale_opt.length());
      double scale;
      arg >> scale;
      if (scale < 1)
        scale = 1;
    } else if (help_opt == argv[i]) {
      std::cout << USAGE << std::endl;
      return 0;
    } else if (magickcenter == argv[i]) {
      do_magick_center=true;
    } else {
      input_name.assign(argv[i]);

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

        std::vector<Rect> faces;
        cascade.detectMultiScale(small_image, faces, 1.1, 2, 0, Size(30, 30));

        //std::sort (faces.begin(),faces.end(),face_area_compare);

        float image_width  = static_cast<float>(image.size().width);
        float image_height = static_cast<float>(image.size().height);
        float total_area = image_width * image_height;

        if (!faces.empty()) {
          unsigned int face_count = 0;
          //std::vector<Rect>::const_iterator max_region = faces.end();
          for (std::vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++) {
            float area = r->width * r->height;
            if ((area / total_area)<0.05) {
              // face is too small
              continue;
            }

            face_count++;

            // calculate offset to center face in the image
            // Want to enable: http://www.imagemagick.org/discourse-server/viewtopic.php?f=1&t=20823
            float center_face_offset_x = (image_width/ 2.0)-(r->x+(static_cast<float>(r->width)/2.0));
            float center_face_offset_y = (image_height/2.0)-(r->y+(static_cast<float>(r->height)/2.0));

            if (do_magick_center) {
              // imagemagick centering output
              std::cout
                << "convert " 
                << "'" << input_name << "'"
                << " -page "
                << (center_face_offset_x>0 ? "+" : "") 
                << static_cast<int>(center_face_offset_x)
                << (center_face_offset_x>0 ? "+" : "") 
                << static_cast<int>(center_face_offset_y) << " "
                << "-background none -flatten "
                << "'" << input_name << ".centered.jpg" << "'"
                << std::endl;              
            } else {
              // normal output
              std::cout
                << input_name <<  "\t"
                << face_count <<  "\t"
                << area / total_area <<  "\t"
                << r->x / image_width <<  "\t"
                << r->y / image_height <<  "\t"
                << r->width / image_width <<  "\t"
                << r->height / image_height << "\t"
                << std::endl;
            }
          }
        }
      }
    }
  }

  return 0;
}
