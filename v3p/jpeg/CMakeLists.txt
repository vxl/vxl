#
# ./v3p/jpeg/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#
ADD_LIBRARY( jpeg jpeg_sources )

SOURCE_FILES( jpeg_sources
jcapimin
jcapistd
jccoefct
jccolor
jcdctmgr
jchuff
jcinit
jcmainct
jcmarker
jcmaster
jcomapi
jcparam
jcphuff
jcprepct
jcsample
jctrans
jdapimin
jdapistd
jdatadst
jdatasrc
jdcoefct
jdcolor
jddctmgr
jdhuff
jdinput
jdmainct
jdmarker
jdmaster
jdmerge
jdphuff
jdpostct
jdsample
jdtrans
jerror
jfdctflt
jfdctfst
jfdctint
jidctflt
jidctfst
jidctint
jidctred
jquant1
jquant2
jutils
jmemmgr
jmemansi
)
