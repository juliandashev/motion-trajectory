add_library(MotionTrajectoryCore STATIC
    VideoProcessor.cc
)

target_link_libraries(MotionTrajectoryCore
    PUBLIC
        MotionTrajectoryHeaders
        ${OpenCV_LIBS}
)
