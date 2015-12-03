# v3p/CMakeLists.txt

PROJECT( vthreep )

SET( VTHREEP_INCLUDE_DIR ${vthreep_SOURCE_DIR} CACHE STATIC "v3p include files" )
SET( VTHREEP_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/v3p CACHE STATIC "v3p include files" )

# The borland compiler produces lots of warnings like
# Warning W8004 C:\cygwin\tmp\vxl_nightly_borland\vxl_src\v3p\tiff\tif_fax3.c 256:
#      'a0' is assigned a value that is never used in function Fax3Decode1D
# There is no point in us fixing them locally. IMS
IF (BORLAND)
  SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w-8004")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w-8004")
ENDIF (BORLAND)


IF (BUILD_CORE_NUMERICS)
  add_subdirectory(netlib)
ENDIF (BUILD_CORE_NUMERICS)

IF (BUILD_VGUI OR BUILD_FOR_VXL_DASHBOARD)
  add_subdirectory(Qv)
ENDIF (BUILD_VGUI OR BUILD_FOR_VXL_DASHBOARD)

IF (BUILD_CORE_GEOMETRY)
  OPTION( VXL_FORCE_V3P_CLIPPER "Use V3P instead of any native Clipper library?" YES)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_CLIPPER )
  add_subdirectory(clipper)
ENDIF (BUILD_CORE_GEOMETRY)

IF (BUILD_CORE_IMAGING)
  OPTION( VXL_FORCE_V3P_ZLIB "Use V3P instead of any native ZLIB library?" NO)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_ZLIB )
  add_subdirectory(zlib)

  OPTION( VXL_FORCE_V3P_BZLIB2 "Use V3P instead of any native BZip2 library?" NO)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_BZLIB2 )
  add_subdirectory(bzlib)

  OPTION( VXL_FORCE_V3P_JPEG "Use V3P instead of any native JPEG library?" NO)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_JPEG )
  add_subdirectory(jpeg)

  OPTION( VXL_FORCE_V3P_J2K "Use V3P instead of any native JPEG2000 library? (WIN32 only)" NO)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_J2K )
  # Use old CMake policy to avoid warning about directory without CMakeLists.txt
  CMAKE_POLICY(SET CMP0014 OLD)
  add_subdirectory(j2k)

  OPTION( VXL_FORCE_V3P_TIFF "Use V3P instead of any native TIFF library?" NO)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_TIFF )
  add_subdirectory(tiff)

  OPTION( VXL_FORCE_V3P_GEOTIFF "Use V3P instead of any native GEOTIFF library?" NO)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_GEOTIFF )
  add_subdirectory(geotiff)

  OPTION( VXL_FORCE_V3P_PNG "Use V3P instead of any native PNG library?" NO)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_PNG )
  add_subdirectory(png)

  OPTION( VXL_FORCE_V3P_DCMTK "Use V3P instead of any native DCMTK library?" NO)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_DCMTK )
  add_subdirectory(dcmtk)

  OPTION( VXL_FORCE_V3P_OPENJPEG2 "Use V3P instead of any native OpenJPEG v2 library?" NO)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_OPENJPEG2 )
  add_subdirectory(openjpeg2)

  add_subdirectory(rply)

ENDIF (BUILD_CORE_IMAGING)
