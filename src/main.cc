#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <path_to_video>" << std::endl;
    return -1;
  }

  std::string videoPath = argv[1];

  cv::VideoCapture cap(videoPath);
  if (!cap.isOpened()) {
    std::cerr << "Error: Cannot open video file: " << videoPath << std::endl;
    return -1;
  }

  cv::Mat frame, gray, binary, display;
  const cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, {3, 3});

  std::vector<cv::Point> trajectory;

  while (true) {
    if (!cap.read(frame)) {
      std::cout << "End of video." << std::endl;
      break;
    }

    // 1. Convert to grayscale
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // 2. Threshold (sun is very bright)
    cv::threshold(gray, binary, 210, 255, cv::THRESH_BINARY);
    cv::morphologyEx(binary, binary, cv::MORPH_OPEN, kernel);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    if (!contours.empty()) {
      // Find largest contour (Sun)
      size_t largestIdx = 0;
      double maxArea = 0.0;

      for (size_t i = 0; i < contours.size(); ++i) {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea) {
          maxArea = area;
          largestIdx = i;
        }
      }

      // Compute centroid using moments
      cv::Moments m = cv::moments(contours[largestIdx]);
      if (m.m00 != 0) {
        int cx = static_cast<int>(m.m10 / m.m00);
        int cy = static_cast<int>(m.m01 / m.m00);

        cv::Point center(cx, cy);
        trajectory.push_back(center);

        // Draw center
        cv::circle(frame, center, 4, cv::Scalar(0, 0, 255), -1);

        // Draw trajectory
        for (size_t i = 1; i < trajectory.size(); ++i) {
          cv::line(frame, trajectory[i - 1], trajectory[i],
                   cv::Scalar(0, 255, 0), 2);
        }
      }
    }

    // Show results
    cv::resize(binary, display, cv::Size(), 0.6, 0.6);
    cv::imshow("Binary", display);

    char key = (char)cv::waitKey(30);
    if (key == 27) {  // press ESC to stop
      break;
    }
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}
