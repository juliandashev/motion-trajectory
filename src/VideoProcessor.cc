#include "VideoProcessor.hpp"

#include <cmath>
#include <opencv2/opencv.hpp>
#include <vector>

constexpr int REQUIRED_ELLIPSE_FRAMES = 5;
constexpr float TOP_ROI_RATIO = 0.5f;

bool VideoProcessor::findSunCenter(const cv::Mat& frame, cv::Point2f& center) {
  // Process only top part of frame
  int roiHeight = static_cast<int>(TOP_ROI_RATIO * frame.rows);

  cv::Rect roi(0, 0, frame.cols, roiHeight);
  cv::Mat frameROI = frame(roi);

  // Segmentation
  cv::Mat gray, binary, cleaned;
  cv::cvtColor(frameROI, gray, cv::COLOR_BGR2GRAY);
  cv::threshold(gray, binary, 200, 255, cv::THRESH_BINARY);

  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7));

  cv::morphologyEx(binary, cleaned, cv::MORPH_OPEN, kernel);
  cv::morphologyEx(cleaned, cleaned, cv::MORPH_CLOSE, kernel);

  // Find contours
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(cleaned, contours, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_SIMPLE);

  if (contours.empty()) {
    return false;
  }

  // Select largest contour
  int bestIdx = -1;
  double maxArea = 0.0;

  for (std::size_t i = 0; i < contours.size(); ++i) {
    double area = cv::contourArea(contours[i]);
    if (area > maxArea) {
      maxArea = area;
      bestIdx = static_cast<int>(i);
    }
  }

  if (bestIdx < 0) {
    return false;
  }

  const auto& contour = contours[bestIdx];

  cv::RotatedRect ellipse = cv::fitEllipse(contour);

  float a = ellipse.size.width * 0.5f;
  float b = ellipse.size.height * 0.5f;

  float axisRatio = std::min(a, b) / std::max(a, b);

  if (axisRatio < 0.7f) {
    return false;
  }

  // Center of gravity
  cv::Moments m = cv::moments(contour);
  if (m.m00 < 1e-6) {
    return false;
  }

  cv::Point2f cog(static_cast<float>(m.m10 / m.m00),
                  static_cast<float>(m.m01 / m.m00));

  // Check if CoG is inside ellipse
  float angle = ellipse.angle * CV_PI / 180.0f;
  float cosA = std::cos(angle);
  float sinA = std::sin(angle);

  float dx = cog.x - ellipse.center.x;
  float dy = cog.y - ellipse.center.y;

  float xRot = dx * cosA + dy * sinA;
  float yRot = -dx * sinA + dy * cosA;

  float ellipseEq = (xRot * xRot) / (a * a) + (yRot * yRot) / (b * b);

  if (ellipseEq > 1.0f) {
    return false;
  }

  // Output center (global coords
  center = cv::Point2f(cog.x, cog.y + roi.y);

  return true;
}
