#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "Trajectory.hpp"
#include "VideoProcessor.hpp"

int main(int argc, char** argv) {
  std::string videoPath = (argc >= 2) ? argv[1] : "T2_video_moon.mp4";

  cv::VideoCapture cap(videoPath);

  if (!cap.isOpened()) {
    std::cerr << "ERROR: Cannot open video: " << videoPath << "\n";
    return 1;
  }

  int W = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
  int H = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
  double fps = cap.get(cv::CAP_PROP_FPS);

  cv::VideoWriter writer("out/out_trajectory.mp4",
                         cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps,
                         cv::Size(W, H));

  if (!writer.isOpened()) {
    std::cerr << "ERROR: Cannot open output video\n";
    return 1;
  }

  VideoProcessor processor;
  Trajectory trajectory;

  cv::Mat frame;
  cv::Point2f prevCenter;
  bool havePrev = false;

  // used to avoid startup spikes
  int stableCount = 0;
  const int REQUIRED_STABLE_FRAMES = 5;

  while (cap.read(frame)) {
    cv::Point2f center;
    bool found = processor.findSunCenter(frame, center);

    if (found) {
      stableCount++;

      // wait until detection stabilizes
      if (stableCount >= REQUIRED_STABLE_FRAMES) {
        trajectory.addPoint(center);

        if (havePrev) {
          trajectory.addVelocity(center - prevCenter);
        }

        prevCenter = center;
        havePrev = true;

        // draw detected center
        cv::circle(frame, center, 6, cv::Scalar(0, 0, 255), -1);
      }
    }

    // draw measured trajectory
    const auto& pts = trajectory.points();
    for (std::size_t i = 1; i < pts.size(); ++i) {
      cv::line(frame, pts[i - 1], pts[i], cv::Scalar(255, 0, 0), 2);
    }

    // write frame to output video
    writer.write(frame);

    cv::imshow("tracking", frame);
    if (cv::waitKey(30) == 27) break;
  }

  trajectory.extrapolateEuler(W, H);

  cv::Mat finalFrame(H, W, CV_8UC3, cv::Scalar(0, 0, 0));

  auto pts = trajectory.points();
  std::size_t predStart = trajectory.predictionStart();

  for (std::size_t i = 1; i < pts.size(); ++i) {
    cv::Scalar color =
        (i < predStart) ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 0, 255);

    cv::line(finalFrame, pts[i - 1], pts[i], color, 2);
  }

  cv::Mat display;

  cv::resize(finalFrame, display, cv::Size(), 0.5, 0.5);
  cv::imshow("final trajectory", display);

  cv::waitKey(0);

  std::ofstream out("out/trajectory.txt");

  for (const auto& p : pts) {
    out << p.x << " " << p.y << "\n";
  }
  out.close();

  writer.release();
  cap.release();
  cv::destroyAllWindows();

  std::cout << "DONE. Output saved to out_trajectory.mp4 and trajectory.txt\n";
  return 0;
}
