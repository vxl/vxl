ADD_EXECUTABLE( vul_sequence_filename_example vul_sequence_filename_example.cxx )
TARGET_LINK_LIBRARIES( vul_sequence_filename_example vul )

ADD_EXECUTABLE( vul_psfile_example vul_psfile_example.cxx )
TARGET_LINK_LIBRARIES( vul_psfile_example vul )

ADD_EXECUTABLE( expand_path expand_path.cxx )
TARGET_LINK_LIBRARIES( expand_path vul )
