LINK_DIRECTORIES( conversions vxl gel )
USE_NETLIB := 1
LINK_LIBRARIES( vdgl vsol vgl vnl-algo vnl vbl )

IF(HAS_SO)
  LINK_DIRECTORIES( COOL GeneralUtility SpatialObjects Numerics )
  LINK_LIBRARIES( DigitalGeometry CompGeom Topology Groups Geometry SpatialBasics Basics Solver Numerics )
  ADD_EXECUTABLE( test_DigitalCurve_to_vdgl )
ENDIF(HAS_SO)
