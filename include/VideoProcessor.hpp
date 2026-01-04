#pragma once

#include <opencv2/opencv.hpp>

class VideoProcessor {
 public:
  // Returns true if center is found
  bool findSunCenter(const cv::Mat& frame, cv::Point2f& center);
};
