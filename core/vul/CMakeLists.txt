#
# ./vxl/vul/CMakeLists.txt
#
#   ian.m.scott@stud.man.ac.uk
#


ADD_LIBRARY(vul vul_sources)

SOURCE_FILES( vul_sources
vul_arg
vul_reg_exp
vul_awk
vul_sequence_filename_map
vul_canonical_path
vul_file
vul_sprintf
vul_file_iterator
vul_string
vul_get_timestamp
vul_test
vul_timer
vul_printf
vul_timestamp
vul_psfile
vul_trace
vul_user_info
vul_redirector
)

AUX_SOURCE_DIRECTORY(Templates vul_sources)

