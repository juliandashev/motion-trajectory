#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

class Trajectory {
 public:
  void addPoint(const cv::Point2f& p);
  void addVelocity(const cv::Point2f& v);

  void extrapolateEuler(int width, int height);

  std::size_t predictionStart() const { return m_predictionStart; }

  const std::vector<cv::Point2f>& points() const;

 private:
  std::vector<cv::Point2f> m_points;
  std::vector<cv::Point2f> m_velocities;
  std::size_t m_predictionStart = 0;

  cv::Point2f averageVelocity() const;
};
