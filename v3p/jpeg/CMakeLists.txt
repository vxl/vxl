#
# ./v3p/jpeg/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#
ADD_LIBRARY( jpeg jpeg_sources )

SOURCE_FILES( jpeg_sources
jcapimin  jdapistd  jfdctflt
jcapistd  jdatadst  jfdctfst
jccoefct  jdatasrc  jfdctint
jccolor   jdcoefct  jidctflt
jcdctmgr  jdcolor   jidctfst
jchuff    jddctmgr  jidctint
jcinit    jdhuff    jidctred
jcmainct  jdinput   jmemansi
jcmarker  jdmainct  
jcmaster  jdmarker  
jcomapi   jdmaster  jmemmgr
jcparam   jdmerge   jmemname
jcphuff   jdphuff   jmemnobs
jcprepct  jdpostct  
jcsample  jdsample  jquant1
jctrans   jdtrans   jquant2
jdapimin  jerror    jutils
)
