#!/bin/bash  

# Path to sequence 015 with lossless compresion, it MUST be changed by your own path.
path_video='...your_own_path..../Seq_015/Seq_015.mov'

# Path to config file it is inside of the project so doen't need to be changed,
# modifing its parameters could lead to different results.
path_conf='./Examples/Monocular/Endomapper_endoscope_03_section_54420.yaml'

# Path to ORB vocabulary it is inside of the project so doen't need to be changed.
path_voc='./Vocabulary/ORBvoc.txt'

./Examples/Monocular/mono_endomapper ${path_voc} ${path_conf} ${path_video}
