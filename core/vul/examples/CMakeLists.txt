ADD_EXECUTABLE( vul_sequence_filename_example vul_sequence_filename_example.cxx )
ADD_EXECUTABLE( expand_path expand_path.cxx )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
