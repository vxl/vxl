# brl/bbas/bxml/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

# find the path to the xerces libraries and includes
INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/find_xerces.cmake )

IF(HAS_XERCES)

 # sources that depend on xerces being present
 SET(bxml_sources
     bxml_input_converter.cxx                    bxml_input_converter.h
                                                 bxml_generic_ptr.h
     bxml_vsol_point_2d_input_converter.cxx      bxml_vsol_point_2d_input_converter.h
                                                 bxml_vsol_point_2d_input_converter_sptr.h
                                                 bxml_input_converter_sptr.h
     bxml_io.cxx                                 bxml_io.h
     bxml_vdgl_digital_curve_input_converter.cxx bxml_vdgl_digital_curve_input_converter.h
     bxml_vtol_vertex_2d_input_converter.cxx     bxml_vtol_vertex_2d_input_converter.h
     bxml_vtol_zero_chain_2d_input_converter.cxx bxml_vtol_zero_chain_2d_input_converter.h
     bxml_vtol_edge_2d_input_converter.cxx       bxml_vtol_edge_2d_input_converter.h
     bxml_vtol_io.cxx                            bxml_vtol_io.h
 )
 AUX_SOURCE_DIRECTORY(Templates bxml_sources)

 ADD_LIBRARY(bxml ${bxml_sources})

 TARGET_LINK_LIBRARIES(bxml ${XERCES_LIBRARIES} bdgl vtol vdgl vsol vbl)

 IF(BUILD_TESTING)
   SUBDIRS(tests)
 ENDIF(BUILD_TESTING)

ENDIF(HAS_XERCES)
