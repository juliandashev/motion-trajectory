This project was developed as a course project for the Computer Vision course at Technical University of Sofia – Branch Plovdiv, within the Computer Systems and Technologies program.

The aim of the project is to apply core computer vision concepts for motion detection, object tracking, and trajectory estimation from video streams or sequences of images, for educational and academic purposes.

Build instructions (Linux):

1. OpenCV
    
- Check wether opencv is installed
    ```
    pkg-config --modversion opencv4
    ```
- If not, run this command:
    ```
    sudo apt install libopencv-dev
    ```
2. Source shell script to execute preferred build type 
<[ debug | release | incremental-build ]>
    ```
    source run.sh
    debug
    ```
3. **(Optional)** Enter sudo password to push binary into /usr/bin/ so its available from anywhere, not just inside build folder

4. Usage
    ```
    MotionTrajectory <input_video> <output_video>
    ```
- Example:
    ```
    MotionTrajectory data/T2_video_solar.mp4 out/out_trajectory.mp4
    ```

Video should play automatically tracing the sun's path