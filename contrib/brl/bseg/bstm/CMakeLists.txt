# contrib/brl/bseg/bstm/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(bstm_sources bstm_block_metadata.h bstm_block_metadata.cxx
	         bstm_time_tree.h bstm_time_tree.cxx
		 bstm_scene.h bstm_scene.cxx
		 bstm_scene_parser.h bstm_scene_parser.cxx
		 bstm_data_base.h bstm_data_base.cxx
		 bstm_block.h bstm_block.cxx
		 bstm_data_traits.h
		 bstm_time_block.h bstm_time_block.cxx
		 bstm_util.cxx bstm_util.h)

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )
ENDIF(EXPAT_FOUND)


AUX_SOURCE_DIRECTORY(Templates bstm_sources)

ADD_LIBRARY(bstm  ${bstm_sources})
TARGET_LINK_LIBRARIES(bstm boxm2 bstm_basic vgl vnl_algo vnl)

IF(EXPAT_FOUND)
  TARGET_LINK_LIBRARIES(bstm expatpp)
ENDIF(EXPAT_FOUND)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

#basic
subdirs(basic)

#io
subdirs(io)

#c plus plus library
subdirs(cpp)

#opencl section
subdirs(ocl)

#view section
subdirs(view)

#processes
subdirs(pro)
