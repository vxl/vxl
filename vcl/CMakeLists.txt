#
# ./vcl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

# Set up vcl_where_root_dir.h to have a macro set to $VCL_SOURCE_ROOT_DIR
CONFIGURE_FILE(
      ${allvxl_SOURCE_DIR}/vcl/vcl_where_root_dir.h.in
      ${allvxl_BINARY_DIR}/vcl/vcl_where_root_dir.h ESCAPE_QUOTES)


SOURCE_FILES( vcl_sources
vcl_cmath
vcl_cassert
vcl_cstdlib
vcl_deprecated
emulation/vcl_string_instances
emulation/vcl_straits
emulation/vcl_rbtree_instances
emulation/vcl_hashtable
emulation/vcl_alloc
)

IF(WIN32)
  ADD_DEFINITIONS( -DBUILDING_VCL_DLL )
  INCLUDE_DIRECTORIES( config.win32-vc60 )
ENDIF(WIN32)

AUX_SOURCE_DIRECTORY(Templates vcl_sources)

ADD_LIBRARY(vcl vcl_sources)


IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
