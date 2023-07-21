#!/bin/bash  

# Filename of the video to be processed with its absolute path. 
path_video='...your_own_path..../Seq_015/Seq_015.mov'

# Relative path filename to the config file, the calibration parameters are tuned to those of Endoscope_03, the
# one used in Seq_015.mov. If you process another sequence you might need to update the
# calibration parameters. 
# The section of frames to be processed is detailed in this file, in this example 54420-55170
path_conf='./Examples/Monocular/Endomapper_endoscope_03_section_54420.yaml'

# Relative path filename to ORB vocabulary.
path_voc='./Vocabulary/ORBvoc.txt'

./Examples/Monocular/mono_endomapper ${path_voc} ${path_conf} ${path_video}
