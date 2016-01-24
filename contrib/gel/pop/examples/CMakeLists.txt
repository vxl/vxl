# gel/pop/examples/CMakeLists.txt

if( WIN32 )
  add_definitions( -D_AFXDLL )
  set( CMAKE_MFC_FLAG 6 )
endif()

add_executable( pop_simple_exec pop_simple_exec.cxx )
target_link_libraries( pop_simple_exec pop)
