# contrib/brl/bseg/bmdl_batch/CMakeLists.txt

#INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bmdl )

SET(boct_sources
    boct_loc_code.cxx           boct_loc_code.h
    boct_tree_cell.txx          boct_tree_cell.h
    boct_tree.txx               boct_tree.h
    boct_test_util.txx          boct_test_util.h
)

AUX_SOURCE_DIRECTORY(Templates boct_sources)

ADD_LIBRARY(boct  ${boct_sources})

TARGET_LINK_LIBRARIES(boct vcl vgl vgl_io vbl)

#tests
SUBDIRS(tests)

