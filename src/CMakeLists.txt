add_library(MotionTrajectoryCore STATIC
    Trajectory.cc
    VideoProcessor.cc
)

target_link_libraries(MotionTrajectoryCore
    PUBLIC
        MotionTrajectoryHeaders
        ${OpenCV_LIBS}
)
