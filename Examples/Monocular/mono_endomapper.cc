/**
* This file is part of ORB-SLAM3
*
* Copyright (C) 2017-2021 Carlos Campos, Richard Elvira, Juan J. Gómez Rodríguez, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
* Copyright (C) 2014-2016 Raúl Mur-Artal, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
*
* ORB-SLAM3 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM3 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
* the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with ORB-SLAM3.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>

#include<opencv2/core/core.hpp>

#include<System.h>

using namespace std;


int main(int argc, char **argv)
{  
    if(argc != 4)
    {
        cerr << endl << "Usage: ./mono_endomapper path_to_vocabulary path_to_settings path_to_sequence_video" << endl;
        return 1;
    }

    // Load the specific configuration data
    cv::FileStorage fSettings(argv[2], cv::FileStorage::READ);
    cv::FileNode node = fSettings["Camera.numSkippedFrames"];
    int nSkippedFrames = 0;
    if(!node.empty() && node.isInt())
    {
        nSkippedFrames = node.operator int();
    }
    if(nSkippedFrames < 0)
    {
        std::cerr << "*Camera.numSkippedFrames is less than 0, it must be a positive value*" << std::endl;
        return -1;
    }
    
    node = fSettings["Camera.fps"];
    int nFps = 50;
    if(!node.empty() && node.isInt())
    {
        nFps = node.operator int();
    }
    
    // Get initial time deffined in setting file
    int initial_second = 0;
    int initial_minute = 0;
    node = fSettings["System.initialTimeSecond"];
    if(!node.empty() && node.isInt())
    {
        initial_second = node.operator int();
    }

    node = fSettings["System.initialTimeMinute"];
    if(!node.empty() && node.isInt())
    {
        initial_minute = node.operator int();
    }

    if(initial_second < 0 || initial_second >= 60)
    {
        std::cerr << "*System.initialTimeSecond has an incorrect value: " << initial_second << ". It must be >= 0 and < 60*" << std::endl;
        return -1;
    }

    if(initial_minute < 0)
    {
        std::cerr << "*System.initialTimeMinute has an incorrect value: " << initial_minute << ". It must be >= 0*" << std::endl;
        return -1;
    }

    // Get initial time deffined in setting file
    int finish_second = 0;
    int finish_minute = 0;
    node = fSettings["System.finishTimeSecond"];
    if(!node.empty() && node.isInt())
    {
        finish_second = node.operator int();
    }

    node = fSettings["System.finishTimeMinute"];
    if(!node.empty() && node.isInt())
    {
        finish_minute = node.operator int();
    }

    if(finish_second < 0 || finish_second >= 60)
    {
        std::cerr << "*System.finishTimeSecond has an incorrect value: " << finish_second << ". It must be >= 0 and < 60*" << std::endl;
        return -1;
    }

    if(initial_minute < 0)
    {
        std::cerr << "*System.finishTimeMinute has an incorrect value: " << finish_minute << ". It must be >= 0*" << std::endl;
        return -1;
    }

    double vid_initial_time_millisec = 0;
    if(initial_second != 0 || initial_minute != 0)
    {
        vid_initial_time_millisec = ((double)((initial_minute * 60) + initial_second)) *1e3;
        std::cout << "Sequence initial time is " << std::setw(2) << std::setfill('0') << initial_minute << ":" << initial_second << std::endl;
        std::cout << " which in millisecons is: " << vid_initial_time_millisec << std::endl;
    }

    double vid_finish_time_millisec = 0;
    if(finish_second != 0 || finish_minute != 0)
    {
        vid_finish_time_millisec = ((double)((finish_minute * 60) + finish_second)) *1e3;
        if(vid_initial_time_millisec > vid_finish_time_millisec)
        {
            std::cout << "Invalid finish time, initial time is greater than the finish time... Hence, finish time is ignored" << std::endl;
            vid_finish_time_millisec = 0;
        }
        else
        {
            std::cout << "Sequence finish time is " << std::setw(2) << std::setfill('0') << finish_minute << ":" << finish_second << std::endl;
            std::cout << " which in millisecons is: " << vid_finish_time_millisec << std::endl;
        }
    }

    int initial_frame = 0;
    int finish_frame = 0;
    node = fSettings["System.initialNumberFrame"];
    if(!node.empty() && node.isInt())
    {
        initial_frame = node.operator int();
    }

    if(initial_frame < 0)
    {
        std::cerr << "*System.initialNumberFrame has an incorrect value: " << initial_frame << ". It must be >= 0*" << std::endl;
        return -1;
    }

    node = fSettings["System.finishNumberFrame"];
    if(!node.empty() && node.isInt())
    {
        finish_frame = node.operator int();
    }

    if(finish_frame < 0)
    {
        std::cerr << "*System.finishNumberFrame has an incorrect value: " << finish_frame << ". It must be >= 0*" << std::endl;
        return -1;
    }


    if(initial_frame != 0 && finish_frame != 0 && finish_frame < initial_frame)
    {
        std::cout << "Invalid finish frame number, initial frame number is greater than the finish one... Hence, finish frame number is ignored" << std::endl;
        finish_frame = 0;
    }
    
    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    
    cout << endl << "-------" << endl;
    cout.precision(17);

    // Load video file
    cv::Mat frame;
    cv::VideoCapture vid_cap;
    if(!vid_cap.open(string(argv[3])))
    {
        cerr << "Error: Video file " << argv[3] << " doesn't exist" << endl;
        return -1;
    }

    float dT = 1.f/nFps;
    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM3::System SLAM(argv[1],argv[2],ORB_SLAM3::System::MONOCULAR, true);
    
    double vid_millisec = vid_cap.get(cv::CAP_PROP_POS_MSEC);
    int vid_pos_frame = vid_cap.get(cv::CAP_PROP_POS_FRAMES);

    bool bUseNumber = false;
    bool bUseTime = false;
    if(initial_frame > 0)
    {
        vid_cap.set(cv::CAP_PROP_POS_FRAMES, initial_frame);
        bUseNumber = true;
    }
    else if(vid_initial_time_millisec > 0)
    {
        vid_cap.set(cv::CAP_PROP_POS_MSEC, vid_initial_time_millisec);
        bUseTime = true;
    }

    double startVid_ms = vid_cap.get(cv::CAP_PROP_POS_MSEC);
    std::cout << "Start time: " << startVid_ms << std::endl;
    vector<ORB_SLAM3::IMU::Point> vImuMeas;

    while(vid_cap.read(frame))
    {
        vid_millisec = vid_cap.get(cv::CAP_PROP_POS_MSEC);
        vid_pos_frame = vid_cap.get(cv::CAP_PROP_POS_FRAMES);

        string frame_name = to_string(vid_pos_frame);
    
        if(bUseNumber)
        {
            if(finish_frame > 0 && vid_pos_frame >= finish_frame)
            {
                // Execution must be stopped
                break;
            }
        }
        else if(bUseTime)
        {
            if(vid_finish_time_millisec > 0 && vid_millisec >= vid_finish_time_millisec)
            {
                // Execution must be stopped
                break;
            }
        }

        if(nSkippedFrames > 0)
        {
            if(vid_pos_frame % (nSkippedFrames + 1) != 0)
            {
                // It avoids the amount of frames selected
                continue;
            }
        }

        if(frame.empty())
        {
            std::cerr << "Frame lost..." << std::endl;
            continue;
        }

        float tframe_sec = vid_millisec * 1e-3;

        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

        // Pass the image to the SLAM system
        SLAM.TrackMonocular(frame, tframe_sec, vImuMeas, frame_name, vid_millisec, vid_pos_frame);

        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

        double ttrack = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();

        vTimesTrack.push_back(ttrack);

        if(ttrack<dT) {
            usleep((dT-ttrack)*1e6);
        }

    }

    std::sort(vTimesTrack.begin(), vTimesTrack.end());
    double median_track = vTimesTrack[vTimesTrack.size()/2] * 1e3;
    double total_track = 0.f;
    for(int i=0; i<vTimesTrack.size(); ++i)
    {
        total_track += vTimesTrack[i];
    }
    double mean_track = total_track / vTimesTrack.size();
    double accum = 0;
    for(int i=0; i<vTimesTrack.size(); ++i)
    {
        accum += pow(vTimesTrack[i] - mean_track, 2);
    }
    double std_track = sqrt(accum / vTimesTrack.size()) * 1e3;
    mean_track = mean_track * 1e3;

    cout << " Track temporal cost " << endl;
    cout << "---------------------" << endl;
    cout << "  - Track time to real time (ms): " << (dT * 1e3) << endl;
    cout << "  - Median track (ms): " << median_track << endl;
    cout << "  - Mean track (ms): " << mean_track << endl;
    cout << "  - STD track (ms): " << std_track << endl << endl;

    // Stop all threads
    SLAM.Shutdown();

    string name_file = "Atlas_Stats";
    SLAM.SaveAtlasStats(name_file);

    return 0;
}
