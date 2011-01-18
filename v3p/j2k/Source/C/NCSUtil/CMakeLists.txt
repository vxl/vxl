project( NCSUtil )



set(SOURCES 
  CNCSBase64Coder.cpp
  CNCSMetabaseEdit.cpp
  CNCSMultiSZ.cpp
  dynamiclib.c
  error.c
  exception_catch.c
  file.c
  log.cpp
  main.c
  malloc.c
  mutex.c
  NCSBase64.cpp
  NCSCoordinateConverter.cpp
  NCSCoordinateSystem.cpp
  NCSCoordinateTransform.cpp
  NCSCrypto.cpp
  NCSError.cpp
  NCSEvent.cpp
  NCSExtent.cpp
  NCSExtents.cpp
  NCSLog.cpp
  NCSMutex.cpp
  NCSObject.cpp
  NCSObjectList.cpp
  NCSPoint.cpp
  NCSPrefs.cpp
  NCSPrefsWin.cpp
  NCSRasterCoordinateConverter.cpp
  NCSScreenPoint.cpp
  NCSServerState.cpp
  NCSString.cpp
  NCSThread.cpp
  ncsutil.rc
  NCSWorldPoint.cpp
  pool.c
  quadtree.cpp
  queue.c
  thread.c
  timer.c
  util.c
  NCSUtil.def
  #headers
  
CNCSBase64Coder.h
../../include/CNCSMetabaseEdit.h
../../include/CNCSMultiSZ.h
../../include/NCSArray.h
../../include/NCSBase64.h
../../include/NCSCoordinateConverter.h
../../include/NCSCoordinateSystem.h
../../include/NCSCoordinateTransform.h
../../include/NCSCrypto.h
../../include/NCSDefs.h
../../include/NCSDynamicLib.h
../../include/NCSError.h
../../include/NCSErrors.h
../../include/NCSEvent.h
../../include/NCSExtent.h
../../include/NCSExtents.h
../../include/NCSFile.h
../../include/NCSFileIO.h
../../include/NCSLog.h
../../include/NCSMalloc.h
../../include/NCSMemPool.h
../../include/NCSMisc.h
../../include/NCSMutex.h
../../include/NCSObject.h
../../include/NCSObjectList.h
../../include/NCSPackets.h
../../include/NCSPoint.h
../../include/NCSPrefs.h
./NCSPrefsWin.h		
./NCSPrefsXML.h
../../include/NCSQuadTree.h
../../include/NCSQueue.h
../../include/NCSScreenPoint.h
../../include/NCSServerState.h
../../include/NCSThread.h
../../include/NCSTimeStamp.h
../../include/NCSTypes.h
../../include/NCSUtil.h
../../include/NCSWorldPoint.h
resource.h
)

include_directories( 
  ../../include
)

add_definitions( 
  -DNCSMALLOCLOG
  -DNCSUTIL_EXPORTS
  -DNDEBUG
  -DWIN32
  -D_USRDLL
  -D_WINDOWS
)

add_library( NCSUtil SHARED ${SOURCES} )
SET_TARGET_PROPERTIES(NCSUtil PROPERTIES OUTPUT_NAME NCSUtil DEBUG_POSTFIX d)
INSTALL_TARGETS( /lib NCSUtil )
target_link_libraries( NCSUtil winmm odbc32 odbccp32 version imagehlp shlwapi )
