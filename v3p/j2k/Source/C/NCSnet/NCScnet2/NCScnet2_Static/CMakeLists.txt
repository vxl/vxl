project( NCScnet2_Static )

set(SOURCES 
  ../cnet2util.c
  ../connect.c
  ../NCScnet.c
  ../NCSWinHttp.c
  ../packet.c
  ../cnet.h
  ../../../../include/NCScnet.h
  ../../../../include/NCSnet.h
  ../NCSWinHttp.h
)

include_directories( 
  ../../../../include
)

add_definitions( 
  -DNCSECW_STATIC_LIBS
  -DNDEBUG
  -D_LIB
)

add_library( NCScnet2S STATIC ${SOURCES} )

