# This is mul/ipts/tools/CMakeLists.txt

LINK_LIBRARIES( ipts vul )
ADD_EXECUTABLE( find_dog_peaks find_dog_peaks.cxx )
ADD_EXECUTABLE( find_scale_corners find_scale_corners.cxx )
ADD_EXECUTABLE( show_local_entropy show_local_entropy.cxx )
