# v3p/mpeg2/CMakeLists.txt

INCLUDE( ${MODULE_PATH}/FindMPEG2.cmake )

IF( NOT VXL_USING_NATIVE_MPEG2 )

# If RUN_CONFIGURE is used here, configure is not run when cmake is first run.
IF( UNIX )
  EXEC_PROGRAM( ${vthreep_SOURCE_DIR}/mpeg2/configure
                ${vthreep_BINARY_DIR}/mpeg2 )
ENDIF( UNIX )

# A 1st attempt at replacing the MPEG configure script with CMake
# CONFIGURE_FILE(${vthreep_SOURCE_DIR}/mpeg2/include/config_cmake.h.in ${vthreep_BINARY_DIR}/mpeg2/include/config_cmake.h)

IF( UNIX )
  # this is where autoconf will put config.h and inttypes.h
  INCLUDE_DIRECTORIES( ${vthreep_BINARY_DIR}/mpeg2/include )
ELSE( UNIX )
  # location of handwritten config.h and inttypes.h for windows
  INCLUDE_DIRECTORIES( ${vthreep_SOURCE_DIR}/mpeg2/win32 )
ENDIF( UNIX )

INCLUDE_DIRECTORIES( ${vthreep_SOURCE_DIR}/mpeg2/include/mpeg2dec )

SET( mpeg2_sources
  libmpeg2/decode.c
  libmpeg2/header.c
  libmpeg2/stats.c
  libmpeg2/cpu_state.c
  libmpeg2/slice.c
  libmpeg2/motion_comp.c
  libmpeg2/motion_comp_mlib.c
  libmpeg2/motion_comp_mmx.c
  libmpeg2/motion_comp_altivec.c
  libmpeg2/idct.c
  libmpeg2/idct_mlib.c
  libmpeg2/idct_mmx.c
  libmpeg2/idct_altivec.c
)
ADD_LIBRARY( mpeg2 ${mpeg2_sources} )  

SET( vo_sources
  libvo/video_out.c

  # not needed in VXL
  # libvo/video_out_x11.c
  # libvo/video_out_sdl.c
  # libvo/video_out_mga.c

  libvo/video_out_null.c
  libvo/video_out_pgm.c

  libvo/yuv2rgb.c
  libvo/yuv2rgb_mlib.c
  libvo/yuv2rgb_mmx.c 
)
ADD_LIBRARY( vo ${vo_sources} )  

ENDIF( NOT VXL_USING_NATIVE_MPEG2 )
