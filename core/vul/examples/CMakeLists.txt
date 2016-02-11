add_executable( vul_sequence_filename_example vul_sequence_filename_example.cxx )
target_link_libraries( vul_sequence_filename_example ${VXL_LIB_PREFIX}vul )

add_executable( vul_psfile_example vul_psfile_example.cxx )
target_link_libraries( vul_psfile_example ${VXL_LIB_PREFIX}vul )

add_executable( expand_path expand_path.cxx )
target_link_libraries( expand_path ${VXL_LIB_PREFIX}vul )
