# v3p/mpeg2/CMakeLists.txt
INCLUDE( ${MODULE_PATH}/FindMPEG2.cmake )

IF( NOT VXL_USING_NATIVE_MPEG2 )


OPTION(VXL_MPEG_USE_CONFIGURE "Should v3p/mpeg2 setup be done with configure (or cmake if NO)" NO)
MARK_AS_ADVANCED( VXL_MPEG_USE_CONFIGURE )

IF( VXL_MPEG_USE_CONFIGURE )

  # If RUN_CONFIGURE is used here, configure is not run when cmake is first run.
  IF( UNIX )
    EXEC_PROGRAM( ${vthreep_SOURCE_DIR}/mpeg2/configure
                  ${vthreep_BINARY_DIR}/mpeg2 )
  ENDIF( UNIX )

  IF( UNIX )
    # this is where autoconf will put config.h and inttypes.h
    INCLUDE_DIRECTORIES( ${vthreep_BINARY_DIR}/mpeg2/include )
  ELSE( UNIX )
    # location of handwritten config.h and inttypes.h for windows
    INCLUDE_DIRECTORIES( ${vthreep_SOURCE_DIR}/mpeg2/win32 )
  ENDIF( UNIX )

ELSE( VXL_MPEG_USE_CONFIGURE )

  INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake)
  INCLUDE (${CMAKE_ROOT}/Modules/CheckSymbolExists.cmake)
  INCLUDE (${vxl_SOURCE_DIR}/config/cmake/config/vxl_config_macros.cmake)

  IF( CMAKE_SYSTEM_PROCESSOR MATCHES ppc )
    SET( MPEG_ARCH_PPC 1 )
  ELSE( CMAKE_SYSTEM_PROCESSOR MATCHES ppc )
    SET( MPEG_ARCH_PPC 0 )
  ENDIF( CMAKE_SYSTEM_PROCESSOR MATCHES ppc )

  IF( CMAKE_SYSTEM_PROCESSOR MATCHES i[56]86 )
	  SET( MPEG_ARCH_X86 1 )
  ELSE( CMAKE_SYSTEM_PROCESSOR MATCHES i[56]86 )
	  SET( MPEG_ARCH_X86 0 )
  ENDIF( CMAKE_SYSTEM_PROCESSOR MATCHES i[56]86 )
  IF(WIN32)
  # unfortunately MSVC6 (and probably other Windows compilers
  # can't cope with the mmx assembler
    SET( MPEG_ARCH_X86 0 )
  ENDIF(WIN32)


  # Calculate the largest value of __aligned
  
  PERFORM_CHECK_HEADER(inttypes.h MPEG_HAVE_INTTYPES_H)
  PERFORM_C_CHECK_FUNCTION(memalign "stdlib.h" MPEG_HAVE_MEMALIGN)

  ## Still need to figure out how to calculate this for
  ## potentially maximum efficiency
  SET( MPEG_ATTRIBUTE_ALIGNED_MAX 0 )

  CONFIGURE_FILE(${vthreep_SOURCE_DIR}/mpeg2/include/config_cmake.h.in ${vthreep_BINARY_DIR}/mpeg2/include/config.h)
  IF( NOT MPEG_HAVE_INTTYPES_H )
    CONFIGURE_FILE(${vthreep_SOURCE_DIR}/mpeg2/include/inttypes.h.in ${vthreep_BINARY_DIR}/mpeg2/include/inttypes.h)
  ENDIF( NOT MPEG_HAVE_INTTYPES_H )
  INCLUDE_DIRECTORIES( ${vthreep_BINARY_DIR}/mpeg2/include )

ENDIF( VXL_MPEG_USE_CONFIGURE )

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
