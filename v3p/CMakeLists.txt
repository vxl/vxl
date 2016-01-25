# v3p/CMakeLists.txt

PROJECT( vthreep )

SET( VTHREEP_INCLUDE_DIR ${vthreep_SOURCE_DIR} CACHE STATIC "v3p include files" )
SET( VTHREEP_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/v3p CACHE STATIC "v3p include files" )

IF (BUILD_CORE_NUMERICS)
  add_subdirectory(netlib)
ENDIF (BUILD_CORE_NUMERICS)

CMAKE_DEPENDENT_OPTION( VXL_FORCE_V3P_CLIPPER "Use V3P instead of any native Clipper library?" ON
                          "BUILD_CORE_GEOMETRY" OFF)
MARK_AS_ADVANCED( VXL_FORCE_V3P_CLIPPER )
if (VXL_FORCE_V3P_CLIPPER)
  add_subdirectory(clipper)
endif ()

CMAKE_DEPENDENT_OPTION( VXL_FORCE_V3P_ZLIB "Use V3P instead of any native ZLIB library?" OFF
                        "BUILD_CORE_IMAGING" OFF)
MARK_AS_ADVANCED( VXL_FORCE_V3P_ZLIB )
if (VXL_FORCE_V3P_ZLIB)
  add_subdirectory(zlib)
endif()

IF (BUILD_CORE_IMAGING)
  OPTION( VXL_FORCE_V3P_BZLIB2 "Use V3P instead of any native BZip2 library?" OFF)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_BZLIB2 )
  add_subdirectory(bzlib)

  OPTION( VXL_FORCE_V3P_JPEG "Use V3P instead of any native JPEG library?" OFF)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_JPEG )
  add_subdirectory(jpeg)

  OPTION( VXL_FORCE_V3P_J2K "Use V3P instead of any native JPEG2000 library? (WIN32 only)" OFF)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_J2K )
  # Use old CMake policy to avoid warning about directory without CMakeLists.txt
  CMAKE_POLICY(SET CMP0014 OLD)
  add_subdirectory(j2k)

  OPTION( VXL_FORCE_V3P_TIFF "Use V3P instead of any native TIFF library?" OFF)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_TIFF )
  add_subdirectory(tiff)

  OPTION( VXL_FORCE_V3P_GEOTIFF "Use V3P instead of any native GEOTIFF library?" OFF)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_GEOTIFF )
  add_subdirectory(geotiff)

  OPTION( VXL_FORCE_V3P_PNG "Use V3P instead of any native PNG library?" OFF)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_PNG )
  add_subdirectory(png)

  OPTION( VXL_FORCE_V3P_DCMTK "Use V3P instead of any native DCMTK library?" OFF)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_DCMTK )
  add_subdirectory(dcmtk)

  OPTION( VXL_FORCE_V3P_OPENJPEG2 "Use V3P instead of any native OpenJPEG v2 library?" OFF)
  MARK_AS_ADVANCED( VXL_FORCE_V3P_OPENJPEG2 )
  add_subdirectory(openjpeg2)

  add_subdirectory(rply)

ENDIF (BUILD_CORE_IMAGING)
