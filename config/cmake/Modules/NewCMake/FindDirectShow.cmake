# - Test for DirectShow on Windows.
# Once loaded this will define
#   DIRECTSHOW_FOUND     - system has DirectShow
#   DIRECTSHOW_LIBRARIES - libraries you need to link to

# DirectShow is only available on Windows platforms
IF( WIN32 )

# turn off temporarily, until fix is found
#INCLUDE (CheckCXXSourceCompiles)

#SET(CMAKE_REQUIRED_LIBRARIES strmiids quartz)
#CHECK_CXX_SOURCE_COMPILES("
#  #include <atlbase.h>
#  #include <dshow.h>
#  #include <qedit.h>
#
#  int main()
#  {
#    CComPtr<IFilterGraph2> filter_graph;
#    filter_graph.CoCreateInstance(CLSID_FilterGraph);
#    return 0;
#  }
#" DIRECTSHOW_FOUND)
#SET(CMAKE_REQUIRED_LIBRARIES)

SET(DIRECTSHOW_FOUND OFF)

IF( DIRECTSHOW_FOUND )
  SET(DIRECTSHOW_LIBRARIES strmiids quartz)
ENDIF( DIRECTSHOW_FOUND )

ELSE( WIN32 )

SET(DIRECTSHOW_FOUND OFF)

ENDIF( WIN32 )
