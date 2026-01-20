#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "VideoProcessor.hpp"

int main(int argc, char** argv) {
  std::string videoPath = (argc >= 2) ? argv[1] : "T2_video_moon.mp4";
  std::string outputPath = (argc >= 3) ? argv[2] : "out_trajectory.mp4";

  cv::VideoCapture cap(videoPath);

  if (!cap.isOpened()) {
    std::cerr << "ERROR: Cannot open video: " << videoPath << "\n";
    return 1;
  }

  int W = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
  int H = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
  double fps = cap.get(cv::CAP_PROP_FPS);

  cv::VideoWriter writer(outputPath,
                         cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps,
                         cv::Size(W, H));

  if (!writer.isOpened()) {
    std::cerr << "ERROR: Cannot open output video\n";
    return 1;
  }

  VideoProcessor processor;
  std::vector<cv::Point2f> trajectory;

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
        if (havePrev) {
          trajectory.push_back(center);
        }

        prevCenter = center;
        havePrev = true;

        // draw detected center
        cv::circle(frame, center, 6, cv::Scalar(0, 0, 255), -1);
      }
    }

    // draw measured trajectory
    for (std::size_t i = 1; i < trajectory.size(); ++i) {
      cv::line(frame, trajectory[i - 1], trajectory[i], cv::Scalar(255, 0, 0),
               2);
    }

    // write frame to output video
    writer.write(frame);

    cv::imshow("tracking", frame);
    if (cv::waitKey(30) == 27) break;
  }

  cv::waitKey(0);

  writer.release();
  cap.release();
  cv::destroyAllWindows();

  std::cout << "DONE. Output saved to out_trajectory.mp4 and trajectory.txt\n";
  return 0;
}
