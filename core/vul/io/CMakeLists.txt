#
# ./mul/vul/io/CMakeLists.txt
#
#   Ian Scott
#



ADD_LIBRARY(vul_io vul_io_sources)



SOURCE_FILES(vul_io_sources
vul_io_user_info
)


#IF( BUILD_TESTS)
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTS)
