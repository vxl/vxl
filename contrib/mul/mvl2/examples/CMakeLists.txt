# ./mul/mvl2/examples/CMakeLists.txt
#
#   Franck Bettinger
#

link_libraries( mvl2 mbl ${VXL_LIB_PREFIX}vil )

if(NOT WIN32)
  find_package(AVIFile)
  include_directories( ${AVIFILE_INCLUDE_DIR} )
  add_definitions(${AVIFILE_DEFINITIONS} )
else()
  set(AVIFILE_FOUND "YES")
endif()

if(AVIFILE_FOUND)

  add_executable(
    copy_image copy_image.cxx
  )

  add_executable(
    mvl2_to_divx mvl2_to_divx.cxx
  )

endif()
