project(VXL3P_JPEG C)
include( ${VXL_CMAKE_DIR}/FindJPEG.cmake )
set(VXL_JPEG_LIBRARY_NAME jpeg)

if(NOT VXL_USING_NATIVE_JPEG)

include_regular_expression("^(jchuff|jconfig|jdct|jdhuff|jerror|jinclude|jmemsys|jmorecfg|jpegint|jpeglib|jversion|jpeg).*$")

include_directories(BEFORE
               "${CMAKE_CURRENT_BINARY_DIR}"
               "${VXL3P_JPEG_SOURCE_DIR}")

# memmgr back ends: compile only one of these into a working library
# (For now, let's use the mode that requires the image fit into memory.
# This is the recommended mode for Win32 anyway.)
set(systemdependent_SRCS jmemnobs.c)

# library object files common to compression and decompression
set(common_SRCS
jcomapi.c jutils.c jerror.c jmemmgr.c
jaricom.c
)

# compression library object files
set(compression_SRCS
jcapimin.c jcapistd.c jctrans.c jcparam.c jdatadst.c jcinit.c
jcmaster.c jcmarker.c jcmainct.c jcprepct.c jccoefct.c jccolor.c
jcsample.c jchuff.c  jcdctmgr.c jfdctfst.c jfdctflt.c
jfdctint.c
#jcphuff.c
jcarith.c
)

# decompression library object files
set(decompression_SRCS
jdapimin.c jdapistd.c jdtrans.c jdatasrc.c jdmaster.c
jdinput.c jdmarker.c jdhuff.c  jdmainct.c jdcoefct.c
jdpostct.c jddctmgr.c jidctfst.c jidctflt.c jidctint.c
#jidctred.c
jdsample.c jdcolor.c jquant1.c jquant2.c jdmerge.c
#jdphuff.c
jdarith.c
)


if(WIN32)
  if(BUILD_SHARED_LIBS)
    add_definitions(-DJPEGDLL)
  else(BUILD_SHARED_LIBS)
    set(JPEGSTATIC 1)
  endif(BUILD_SHARED_LIBS)
endif(WIN32)

configure_file(${VXL3P_JPEG_SOURCE_DIR}/jpegDllConfig.h.in
               ${VXL3P_JPEG_BINARY_DIR}/jpegDllConfig.h)

vxl_add_library( LIBRARY_NAME jpeg LIBRARY_SOURCES ${systemdependent_SRCS} ${common_SRCS} ${compression_SRCS} ${decompression_SRCS})

set(VXL_JPEG_LIBRARY_NAME jpeg)

endif()
