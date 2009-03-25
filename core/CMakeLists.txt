# vxl/CMakeLists.txt

PROJECT( vxlcore )

SET(global_sources
 vxl_copyright.h
 vxl_version.h
 vxl_config.h.in
)

INSTALL_NOBASE_HEADER_FILES(/include/vxl/core ${global_sources})

# common test executable
SUBDIRS(testlib)

# Tests that check and output the vxl configuration
IF( BUILD_TESTING )
  SUBDIRS( tests )
ENDIF( BUILD_TESTING )


IF (BUILD_CORE_UTILITIES)
  # POSIX layer
  SUBDIRS(vpl)

  # utilities
  SUBDIRS(vul)

  # "basics" (templates)
  SUBDIRS(vbl)
ENDIF (BUILD_CORE_UTILITIES)


# numerics
IF (BUILD_CORE_NUMERICS)
  SUBDIRS(vnl)
ENDIF (BUILD_CORE_NUMERICS)


# geometry (including computational geometry)
IF (BUILD_CORE_GEOMETRY)
  SUBDIRS(vgl)
ENDIF (BUILD_CORE_GEOMETRY)


IF (BUILD_CORE_IMAGING)
  # image
  SUBDIRS(vil)

  IF (BUILD_CORE_NUMERICS)
    SUBDIRS (vil/algo)
  ENDIF (BUILD_CORE_NUMERICS)

  # Old image library
  SUBDIRS(vil1)
ENDIF (BUILD_CORE_IMAGING)

IF (BUILD_CORE_SERIALISATION)
  # Fast Binary IO
  SUBDIRS(vsl)

  IF (BUILD_CORE_UTILITIES)
    SUBDIRS(vul/io vbl/io)
  ENDIF (BUILD_CORE_UTILITIES)

  IF (BUILD_CORE_NUMERICS)
    SUBDIRS(vnl/io vnl/xio)
  ENDIF (BUILD_CORE_NUMERICS)

  IF (BUILD_CORE_GEOMETRY)
    SUBDIRS(vgl/io vgl/xio)
  ENDIF (BUILD_CORE_GEOMETRY)

  IF (BUILD_CORE_IMAGING)
    SUBDIRS(vil/io vil1/io)
  ENDIF (BUILD_CORE_IMAGING)
ENDIF (BUILD_CORE_SERIALISATION)


# coordinate systems
IF (BUILD_CORE_GEOMETRY AND BUILD_CORE_NUMERICS)
  SUBDIRS(vcsl)
ENDIF (BUILD_CORE_GEOMETRY AND BUILD_CORE_NUMERICS)


IF (BUILD_CORE_ALL)
# multi-platform GUI toolkit
  SUBDIRS(vgui)

  IF(BUILD_EXAMPLES)
    SUBDIRS(examples)
  ENDIF(BUILD_EXAMPLES)
ENDIF (BUILD_CORE_ALL)

# video
SET (CORE_VIDEO_FOUND NO CACHE INTERNAL "VXL core video libraries built")
IF (BUILD_CORE_UTILITIES AND BUILD_CORE_IMAGING)
  OPTION (BUILD_CORE_VIDEO "Build VXL's core video libraries" YES)
  IF (BUILD_CORE_VIDEO)
    SUBDIRS(vidl1)
    SUBDIRS(vidl_vil1)
    SUBDIRS(vidl)
    SET (CORE_VIDEO_FOUND YES CACHE INTERNAL "VXL core video libraries built")
  ENDIF (BUILD_CORE_VIDEO)
ENDIF (BUILD_CORE_UTILITIES AND BUILD_CORE_IMAGING)
