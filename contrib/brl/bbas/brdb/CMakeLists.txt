# brl/bbas/brdb/CMakeLists.txt
# The Brown Relational Database Library

SET(brdb_sources
   brdb_value.cxx                    brdb_value.h                    brdb_value_sptr.h
   brdb_value.txx
   brdb_tuple.cxx                    brdb_tuple.h                    brdb_tuple_sptr.h
   brdb_relation.cxx                 brdb_relation.h                 brdb_relation_sptr.h
   brdb_database.cxx                 brdb_database.h                 brdb_database_sptr.h
   brdb_database_manager.cxx         brdb_database_manager.h
   brdb_query.cxx                    brdb_query.h                    brdb_query_aptr.h
   brdb_selection.cxx                brdb_selection.h                brdb_selection_sptr.h
)



AUX_SOURCE_DIRECTORY(Templates brdb_sources)

ADD_LIBRARY(brdb  ${brdb_sources})

# brdb should not depend on any library that uses it
TARGET_LINK_LIBRARIES(brdb vbl vsl)


IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
