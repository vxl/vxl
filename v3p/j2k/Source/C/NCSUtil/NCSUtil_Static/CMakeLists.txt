project( NCSUtil_Static )

set(SOURCES 
  ../CNCSBase64Coder.cpp
  ../CNCSMetabaseEdit.cpp
  ../CNCSMultiSZ.cpp
  ../dynamiclib.c
  ../error.c
  ../exception_catch.c
  ../file.c
  ../log.cpp
  ../main.c
  ../malloc.c
  ../mutex.c
  ../NCSBase64.cpp
  ../NCSCoordinateConverter.cpp
  ../NCSCoordinateSystem.cpp
  ../NCSCoordinateTransform.cpp
  ../NCSCrypto.cpp
  ../NCSError.cpp
  ../NCSEvent.cpp
  ../NCSExtent.cpp
  ../NCSExtents.cpp
  ../NCSLog.cpp
  ../NCSMutex.cpp
  ../NCSObject.cpp
  ../NCSObjectList.cpp
  ../NCSPoint.cpp
  ../NCSRasterCoordinateConverter.cpp
  ../NCSScreenPoint.cpp
  ../NCSServerState.cpp
  ../NCSThread.cpp
  ../NCSUtil.def
  ../NCSUtil.rc
  ../NCSWorldPoint.cpp
  ../pool.c
  ../prefs.c
  ../quadtree.cpp
  ../queue.c
  ../thread.c
  ../timer.c
  ../util.c
  
	../CNCSBase64Coder.h

	../../../include/CNCSMetabaseEdit.h

	../../../include/CNCSMultiSZ.h
	../../../include/NCSArray.h

	../../../include/NCSBase64.h

	../../../include/NCSCoordinateConverter.h

	../../../include/NCSCoordinateSystem.h

	../../../include/NCSCoordinateTransform.h

	../../../include/NCSCrypto.h

	../../../include/NCSDefs.h

	../../../include/NCSDynamicLib.h
	


	../../../include/NCSError.h
	


	../../../include/NCSErrors.h
	


	../../../include/NCSEvent.h
	


	../../../include/NCSExtent.h
	


	../../../include/NCSExtents.h
	


	../../../include/NCSFile.h
	


	../../../include/NCSFileIO.h
	


	../../../include/NCSLog.h
	


	../../../include/NCSMalloc.h
	


	../../../include/NCSMemPool.h
	


	../../../include/NCSMisc.h
	


	../../../include/NCSMutex.h
	


	../../../include/NCSObject.h
	


	../../../include/NCSObjectList.h
	


	../../../include/NCSPackets.h
	


	../../../include/NCSPoint.h
	


	../../../include/NCSPrefs.h
	


	../../../include/NCSQuadTree.h
	


	../../../include/NCSQueue.h
	


	../../../include/NCSScreenPoint.h
	


	../../../include/NCSServerState.h
	


	../../../include/NCSThread.h
	


	../../../include/NCSTimeStamp.h
	


	../../../include/NCSTypes.h
	


	../../../include/NCSUtil.h
	


	../../../include/NCSWorldPoint.h
	


	../resource.h
  
)

include_directories( 
  ../../../include
)

add_definitions( 
  -DNCSECW_STATIC_LIBS
  -DNDEBUG
  -D_LIB
)

add_library( NCSUtilS STATIC ${SOURCES} )

