add_executable(MotionTrajectory
    main.cc
)

target_link_libraries(MotionTrajectory
    PRIVATE
        MotionTrajectoryCore
)

install(
    TARGETS MotionTrajectory
    RUNTIME DESTINATION bin
)
