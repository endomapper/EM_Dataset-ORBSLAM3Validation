# EM_Dataset-ORBSLAM3Validation


This software is a minor variation of ORB-SLAM3 to process EndoMapper dataset sequences.
A full description of ORB-SLAM3 can be found at [ORB-SLAM3](https://github.com/UZ-SLAMLab/ORB_SLAM3). We add the basic information about it and the compiling instructions.

# 1. License

EM_Dataset-ORBSLAM3Validation is released under [GPLv3 license](https://github.com/UZ-SLAMLab/ORB_SLAM3/LICENSE). For a list of all code/library dependencies (and associated licenses), please see [Dependencies.md](https://github.com/UZ-SLAMLab/ORB_SLAM3/blob/master/Dependencies.md).

For a closed-source version of ORB-SLAM3 for commercial purposes, please contact the authors: orbslam (at) unizar (dot) es.

If you use EM_Dataset-ORBSLAM3Validation in an academic work, please cite:
  
    @article{ORBSLAM3_TRO,
      title={{ORB-SLAM3}: An Accurate Open-Source Library for Visual, Visual-Inertial 
               and Multi-Map {SLAM}},
      author={Campos, Carlos AND Elvira, Richard AND G\´omez, Juan J. AND Montiel, 
              Jos\'e M. M. AND Tard\'os, Juan D.},
      journal={IEEE Transactions on Robotics}, 
      volume={37},
      number={6},
      pages={1874-1890},
      year={2021}
     }

# 2. Prerequisites
We have tested the library in **Ubuntu 16.04** and **18.04**, but it should be easy to compile in other platforms. A powerful computer (e.g. i7) will ensure real-time performance and provide more stable and accurate results.

## C++11 or newer Compiler
We use the new thread and chrono functionalities of C++11.

## Pangolin
We use [Pangolin](https://github.com/stevenlovegrove/Pangolin) for visualization and user interface. Dowload and install instructions can be found at: https://github.com/stevenlovegrove/Pangolin.

## OpenCV
We use [OpenCV](http://opencv.org) to manipulate images and features. Dowload and install instructions can be found at: http://opencv.org. **Required at leat 4.4. Tested with OpenCV 4.4.0**.

## Eigen3
Required by g2o (see below). Download and install instructions can be found at: http://eigen.tuxfamily.org. **Required at least 3.1.0**.

## DBoW2 and g2o (Included in Thirdparty folder)
We use modified versions of the [DBoW2](https://github.com/dorian3d/DBoW2) library to perform place recognition and [g2o](https://github.com/RainerKuemmerle/g2o) library to perform non-linear optimizations. Both modified libraries (which are BSD) are included in the *Thirdparty* folder.

# 3. Building ORB-SLAM3 library and examples

Clone the repository:
```
git clone https://github.com/endomapper/EM_Dataset-ORBSLAM3Validation.git EM_Dataset-ORBSLAM3Validation
```

We provide a script `build.sh` to build the *Thirdparty* libraries and *ORB-SLAM3*. Please make sure you have installed all required dependencies (see section 2). Execute on the code folder:
```
cd EM_Dataset-ORBSLAM3Validation
chmod +x build.sh
./build.sh
```

This will create **libORB_SLAM3.so**  at *lib* folder and the executables in *Examples* folder.

# 4. Running endomapper examples

We provide two scripts to launch the full sequence `launch_full_video.sh` and the picked section which is in the paper `launch_section.sh`, both of them are already configured but it is mandatory change the path to the video of sequence 015 with lossless compresion (Seq_015.mov) in the next line on both files:

```
# Path to sequence 015 with lossless compresion, it MUST be changed by your own path.
path_video='...your_own_path..../Seq_015/Seq_015.mov'
```

To launch the section we type:

```
chmod +x launch_section.sh
./launch_section.sh
```

To launch the full video we type:

```
chmod +x launch_full_video.sh
./launch_full_video.sh
```
NOTE: `chmod` only needs to be used once per file.

# 5. Changing visual parameters

Config files allow you to change visual options without recompiling the code, it would be necessary to obtain a good visualization on different screen resolutions.

There are 2 viewers, one for the 3D map and other for the incoming image, the 3d viewer shows the 3D points, the KeyFrames which are represented as a blue pyrimids and the incoming camera pose which respect to the map as green piramid. The size of the points and pyramid lines can be changed with:
```
Viewer.KeyFrameLineWidth: 2.0 
Viewer.PointSize: 4.0 
Viewer.CameraLineWidth: 3.0
```

The incoming image is show on color with its matched points on the 3D map and the size can be scalated by:
```
Viewer.FrameViewerZoom: 2.0
```
