# ./mul/CMakeLists.txt

project( mul )

doxygen_add_book(contrib/mul
  "Manchester University Library overview documentation")

doxygen_add_package(contrib/mul "Manchester University Libraries")

set( VXL_MUL_INCLUDE_DIR ${mul_SOURCE_DIR} CACHE STATIC "Manchester University package include files" )
set( MUL_INCLUDE_DIR ${VXL_MUL_INCLUDE_DIR} CACHE STATIC "Deprecated variable giving Manchester University package include files" )
set( MUL_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/mul CACHE INTERNAL "Manchester University package includes install path" )

include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${PROJECT_BINARY_DIR} )


add_subdirectory(mbl)
add_subdirectory(vpdfl)
add_subdirectory(pdf1d)
add_subdirectory(clsfy)
add_subdirectory(vimt)
add_subdirectory(ipts)
add_subdirectory(vil3d)
add_subdirectory(vimt3d)
add_subdirectory(mvl2)
add_subdirectory(m23d)
add_subdirectory(mmn)
add_subdirectory(mcal)

add_subdirectory(mipa)
add_subdirectory(msm)
add_subdirectory(msdi)
add_subdirectory(mfpf)
add_subdirectory(fhs)


if( VXL_BUILD_MUL_TOOLS )
  add_subdirectory(tools)
endif()
