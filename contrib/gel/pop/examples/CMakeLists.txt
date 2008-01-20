# gel/pop/examples/CMakeLists.txt

IF( WIN32 )
  ADD_DEFINITIONS( -D_AFXDLL )
  SET( CMAKE_MFC_FLAG 6 )
ENDIF( WIN32 )

ADD_EXECUTABLE( pop_simple_exec pop_simple_exec.cxx )
TARGET_LINK_LIBRARIES( pop_simple_exec pop)
