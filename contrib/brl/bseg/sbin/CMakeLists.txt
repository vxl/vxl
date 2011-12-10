# executables for segmentation processing
# brl/bseg/sbin/CMakeLists.txt

INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )
INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/find_xerces.cmake )
INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

IF( HAS_VGUI )
 IF(HAS_XERCES)
  ADD_DEFINITIONS(-DHAS_XERCES)
 ENDIF(HAS_XERCES)

 ADD_EXECUTABLE (seg_vil seg_vil.cxx)
 TARGET_LINK_LIBRARIES(seg_vil segv vgui)

 ADD_EXECUTABLE (seg_misc seg_misc.cxx)
 TARGET_LINK_LIBRARIES(seg_misc segv vgui)
ENDIF( HAS_VGUI )

ADD_EXECUTABLE (negate negate.cxx)
TARGET_LINK_LIBRARIES(negate vul vil vcl)

ADD_EXECUTABLE (vil_convert vil_convert.cxx)
TARGET_LINK_LIBRARIES(vil_convert vul vil vcl)

ADD_EXECUTABLE (generate_tiff_pyramids generate_tiff_pyramids.cxx)
TARGET_LINK_LIBRARIES(generate_tiff_pyramids vpl vul vil)

ADD_EXECUTABLE (generate_tiff_rset generate_tiff_rset.cxx)
TARGET_LINK_LIBRARIES(generate_tiff_rset vul vil)

#New executable to generate rsets for all files in directory
ADD_EXECUTABLE (gen_tiff_rset_dir gen_tiff_rset_dir.cxx)

TARGET_LINK_LIBRARIES(gen_tiff_rset_dir vpl vul vil)

#Executable to jpeg2000 compress images in a directory if ermapper is installed
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindECW.cmake )
IF(ECW_FOUND)
 INCLUDE_DIRECTORIES(${ECW_INCLUDE_DIR})
 ADD_EXECUTABLE (compress_j2k_dir compress_j2k_dir.cxx)
 TARGET_LINK_LIBRARIES(compress_j2k_dir vpl vul vil)
ENDIF(ECW_FOUND)
