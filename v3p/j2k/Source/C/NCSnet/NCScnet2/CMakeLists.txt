project( NCScnet2 )

SUBDIRS (../../NCSUtil)

set(SOURCES
  cnet2util.c
  connect.c
  NCScnet.c
  NCSWinHttp.c
  packet.c
  NCScnet.rc
  cnet.h
  ../../../include/NCScnet.h
  ../../../include/NCSnet.h
  NCSWinHttp.h
  NCScnet.def
)

include_directories(
  ../../../include
)

add_definitions(
  -DNDEBUG
  -DNOASSEMBLER
)
if (UNIX)
add_definitions( -DLINUX)
ENDIF(UNIX)

if (APPLE)
add_definitions( -DMACINTOSH)
ENDIF(APPLE)
add_library( NCScnet2 SHARED ${SOURCES} )
SET_TARGET_PROPERTIES(NCScnet2 PROPERTIES OUTPUT_NAME NCScnet2 DEBUG_POSTFIX d)
target_link_libraries( NCScnet2 Crypt32 odbc32 odbccp32 wininet wsock32 winmm NCSUtil )
INSTALL_TARGETS( /lib NCScnet2 )
