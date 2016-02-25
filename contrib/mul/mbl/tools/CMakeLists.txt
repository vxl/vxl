# This is mul/mbl/tools/CMakeLists.txt

link_libraries( mbl ${VXL_LIB_PREFIX}vul )

add_executable( mbl_print_log_properties     print_log_properties.cxx )
add_executable( mbl_sample_stats             sample_stats.cxx)
add_executable( mbl_mask_logic               mbl_mask_logic.cxx )
add_executable( mbl_apply_mask_to_file       mbl_apply_mask_to_file.cxx )
add_executable( mbl_masked_file_merge        mbl_masked_file_merge.cxx )
add_executable( mbl_mask_on_mask             mbl_mask_on_mask.cxx )
add_executable( mbl_convert_mask_to_indices  mbl_convert_mask_to_indices.cxx)
add_executable( mbl_convert_indices_to_mask  mbl_convert_indices_to_mask.cxx)
