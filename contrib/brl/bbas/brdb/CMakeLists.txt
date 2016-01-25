# brl/bbas/brdb/CMakeLists.txt
# The Brown Relational Database Library

set(brdb_sources
   brdb_value.cxx                    brdb_value.h                    brdb_value_sptr.h
   brdb_value.txx
   brdb_tuple.cxx                    brdb_tuple.h                    brdb_tuple_sptr.h
   brdb_relation.cxx                 brdb_relation.h                 brdb_relation_sptr.h
   brdb_database.cxx                 brdb_database.h                 brdb_database_sptr.h
   brdb_database_manager.cxx         brdb_database_manager.h
   brdb_query.cxx                    brdb_query.h                    brdb_query_aptr.h
   brdb_selection.cxx                brdb_selection.h                brdb_selection_sptr.h
)

aux_source_directory(Templates brdb_sources)

add_library(brdb  ${brdb_sources})

# brdb should not depend on any library that uses it
target_link_libraries(brdb vbl_io vbl vsl)

#install the .h .txx and libs
install_targets(/lib brdb)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/brdb/ ${brdb_sources})

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
