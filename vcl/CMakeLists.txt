# ./vcl/CMakeLists.txt

# Set up vcl_where_root_dir.h to have a macro set to $VCL_SOURCE_ROOT_DIR
CONFIGURE_FILE(
      ${allvxl_SOURCE_DIR}/vcl/vcl_where_root_dir.h.in
      ${allvxl_BINARY_DIR}/vcl/vcl_where_root_dir.h ESCAPE_QUOTES
)

ADD_LIBRARY(vcl vcl_sources)

SOURCE_FILES( vcl_sources
  vcl_cmath.cxx
  vcl_cassert.cxx
  vcl_cstdlib.cxx
  vcl_deprecated.cxx
  emulation/vcl_string_instances.cxx
  emulation/vcl_straits.cxx
  emulation/vcl_rbtree_instances.cxx
  emulation/vcl_hashtable.cxx
  emulation/vcl_alloc.cxx
)

IF(WIN32)
  IF(NOT CYGWIN)
    ADD_DEFINITIONS( -DBUILDING_VCL_DLL )
    INCLUDE_DIRECTORIES( config.win32-vc60 )
  ENDIF(NOT CYGWIN)
ENDIF(WIN32)

AUX_SOURCE_DIRECTORY(Templates vcl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vcl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
