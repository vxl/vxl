# contrib/brl/bseg/boct/CMakeLists.txt

set(boct_sources
    boct_loc_code.txx         boct_loc_code.h
    boct_tree_cell.txx        boct_tree_cell.h      boct_tree_cell.cxx
    boct_tree.txx             boct_tree.h
    boct_test_util.txx        boct_test_util.h
    boct_tree_cell_reader.txx boct_tree_cell_reader.h
    boct_bit_tree.cxx         boct_bit_tree.h
    util/boct_clone_tree.h
    util/boct_construct_tree.h
)

aux_source_directory(Templates boct_sources)

add_library(boct  ${boct_sources})

target_link_libraries(boct vcl vgl vgl_io vnl_io vbl vnl)

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

install_targets(/lib boct)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boct ${boct_sources})
