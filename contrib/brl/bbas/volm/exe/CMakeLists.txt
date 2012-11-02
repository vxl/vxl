# contrib/brl/bbas/volm/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/ )

ADD_EXECUTABLE( volm_dummy_matcher
                volm_dummy_matcher.cxx
              )

ADD_EXECUTABLE( volm_query_creation
                volm_query_creation.cxx
              )			  

TARGET_LINK_LIBRARIES( volm_dummy_matcher volm depth_map )
TARGET_LINK_LIBRARIES( volm_query_creation volm depth_map bvrml )

