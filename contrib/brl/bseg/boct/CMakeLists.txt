# contrib/brl/bseg/bmdl_batch/CMakeLists.txt
        
#INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bmdl )
    
SET(boct_sources
    boct_loc_code.cxx      boct_loc_code.h
    boct_tree_cell.cxx     boct_tree_cell.h
    boct_tree.cxx          boct_tree.h
    
    boct_test_util.cxx       boct_test_util.h
)
 
ADD_LIBRARY(boct  ${boct_sources})   
        
TARGET_LINK_LIBRARIES(boct vcl vgl) 

#tests       
SUBDIRS(tests)        
    