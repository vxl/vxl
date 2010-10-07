# This is mul/mbl/tools/CMakeLists.txt

LINK_LIBRARIES( mbl vul )

ADD_EXECUTABLE( mbl_print_log_properties     print_log_properties.cxx )
ADD_EXECUTABLE( mbl_sample_stats             sample_stats.cxx)
ADD_EXECUTABLE( mbl_mask_logic               mbl_mask_logic.cxx )
ADD_EXECUTABLE( mbl_apply_mask_to_file       mbl_apply_mask_to_file.cxx )
ADD_EXECUTABLE( mbl_masked_file_merge        mbl_masked_file_merge.cxx )
ADD_EXECUTABLE( mbl_mask_on_mask             mbl_mask_on_mask.cxx )
ADD_EXECUTABLE( mbl_convert_mask_to_indices  mbl_convert_mask_to_indices.cxx)
ADD_EXECUTABLE( mbl_convert_indices_to_mask  mbl_convert_indices_to_mask.cxx)
