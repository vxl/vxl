# contrib/brl/bseg/boct/CMakeLists.txt

SET(boct_sources
    boct_loc_code.txx         boct_loc_code.h
    boct_tree_cell.txx        boct_tree_cell.h      boct_tree_cell.cxx
    boct_tree.txx             boct_tree.h
    boct_test_util.txx        boct_test_util.h
    boct_tree_cell_reader.txx boct_tree_cell_reader.h
    boct_bit_tree.cxx         boct_bit_tree.h
    util/boct_clone_tree.h
    util/boct_construct_tree.h
)

AUX_SOURCE_DIRECTORY(Templates boct_sources)

ADD_LIBRARY(boct  ${boct_sources})

TARGET_LINK_LIBRARIES(boct vcl vgl vgl_io vnl_io vbl vnl)

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib boct)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boct ${boct_sources})
