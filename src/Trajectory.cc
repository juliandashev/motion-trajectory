#include "Trajectory.hpp"

void Trajectory::addPoint(const cv::Point2f& p) { m_points.push_back(p); }

void Trajectory::addVelocity(const cv::Point2f& v) {
  m_velocities.push_back(v);
  if (m_velocities.size() > 10) m_velocities.erase(m_velocities.begin());
}

cv::Point2f Trajectory::averageVelocity() const {
  if (m_velocities.empty()) return cv::Point2f(0, 0);

  cv::Point2f v(0, 0);
  for (const auto& x : m_velocities) v += x;

  return v * (1.0f / m_velocities.size());
}

void Trajectory::extrapolateEuler(int width, int height) {
  if (m_points.empty() || m_velocities.empty()) return;

  m_predictionStart = m_points.size();

  cv::Point2f v = averageVelocity();
  cv::Point2f p = m_points.back();

  while (p.x >= 0 && p.x < width && p.y >= 0 && p.y < height) {
    p = p + v;
    m_points.push_back(p);
  }
}

const std::vector<cv::Point2f>& Trajectory::points() const { return m_points; }
