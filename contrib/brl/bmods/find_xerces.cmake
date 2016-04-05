#
# xerces 1.4
#
# date  : Dec 07, 2002
# author: J. L. Mundy
#

find_path(XERCES_BXML_INCLUDE_PATH dom/DOM.hpp
          ${CMAKE_SOURCE_DIR}/../xerces-c-src1_4_0/include
          /usr/local/xerces-c-src1_4_0/include
          ${CMAKE_SOURCE_DIR}/v3p/xerces-c-src1_4_0/include
)

find_library(XERCES_BXML_LIBRARY_PATH xerces-c
             /usr/local/xerces-c-src1_4_0/lib
             ${CMAKE_BINARY_DIR}/lib
             ${CMAKE_BINARY_DIR}/v3p/lib
             ${CMAKE_BINARY_DIR}/v3p/xerces-c-src1_4_0
)
if(XERCES_BXML_INCLUDE_PATH)
  if(XERCES_BXML_LIBRARY_PATH)
    if(NOT HAS_XERCES)
      set(HAS_XERCES "YES")
      include_directories( ${XERCES_BXML_INCLUDE_PATH} )
      set( XERCES_LIBRARIES ${XERCES_BXML_LIBRARY_PATH} )
    endif()
  endif()
endif()

# this line used for transit phase
find_path(XERCES_NEW_BXML_INCLUDE_PATH xercesc/dom/Dom.hpp
          ${CMAKE_SOURCE_DIR}/../xerces-c2_2_0/include
)

find_library(XERCES_NEW_BXML_LIBRARY_PATH xerces-c_2
          ${CMAKE_SOURCE_DIR}/../xerces-c2_2_0/lib
          ${CMAKE_BINARY_DIR}/v3p/lib
)

if(XERCES_NEW_BXML_INCLUDE_PATH)
  if(XERCES_NEW_BXML_LIBRARY_PATH)
    if(NOT HAS_NEW_XERCES)
      set(HAS_NEW_XERCES "YES")
      include_directories( ${XERCES_NEW_BXML_INCLUDE_PATH} )
      set( NEW_XERCES_LIBRARIES ${XERCES_NEW_BXML_LIBRARY_PATH} )
    endif()
  endif()
endif()

