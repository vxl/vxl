# This is mul/ipts/tools/CMakeLists.txt

LINK_LIBRARIES( ipts vul )
ADD_EXECUTABLE( find_dog_peaks find_dog_peaks.cxx )
ADD_EXECUTABLE( find_scale_corners find_scale_corners.cxx )
ADD_EXECUTABLE( show_local_entropy show_local_entropy.cxx )
ADD_EXECUTABLE( show_orientation_entropy show_orientation_entropy.cxx )
ADD_EXECUTABLE( find_scale_entropy_peaks find_scale_entropy_peaks.cxx )
# ADD_EXECUTABLE( find_scale_orient_entropy_peaks find_scale_orient_entropy_peaks.cxx )
