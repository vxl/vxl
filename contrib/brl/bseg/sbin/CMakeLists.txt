# executables for segmentation processing
# brl/bseg/sbin/CMakeLists.txt

include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )
include( ${VXL_ROOT_SOURCE_DIR}/contrib/brl/bmods/find_xerces.cmake )
include(${VXL_CMAKE_DIR}/FindTIFF.cmake)

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${GEL_INCLUDE_DIR} )

if( HAS_VGUI )
 if(HAS_XERCES)
  add_definitions(-DHAS_XERCES)
 endif()

 add_executable(seg_vil seg_vil.cxx)
 target_link_libraries(seg_vil segv ${VXL_LIB_PREFIX}vgui)

 add_executable(seg_misc seg_misc.cxx)
 target_link_libraries(seg_misc segv ${VXL_LIB_PREFIX}vgui)
endif()

add_executable(negate negate.cxx)
target_link_libraries(negate ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vcl)

add_executable(vil_convert vil_convert.cxx)
target_link_libraries(vil_convert ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vcl)

add_executable(generate_tiff_pyramids generate_tiff_pyramids.cxx)
target_link_libraries(generate_tiff_pyramids ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil)

add_executable(generate_tiff_rset generate_tiff_rset.cxx)
target_link_libraries(generate_tiff_rset ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil)

add_executable(generate_tiff_rset_from_arg_image generate_tiff_rset_from_arg_image.cxx)
target_link_libraries(generate_tiff_rset_from_arg_image ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vpl)

#New executable to generate rsets for all files in directory
add_executable(gen_tiff_rset_dir gen_tiff_rset_dir.cxx)

target_link_libraries(gen_tiff_rset_dir ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil)

#Executable to jpeg2000 compress images in a directory if ermapper is installed
include( ${VXL_CMAKE_DIR}/NewCMake/FindECW.cmake )
if(ECW_FOUND)
 include_directories(${ECW_INCLUDE_DIR})
 add_executable(compress_j2k_dir compress_j2k_dir.cxx)
 target_link_libraries(compress_j2k_dir ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil)
endif()
