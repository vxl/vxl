#
# ./vxl/vgl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(vgl vgl_sources)

SOURCE_FILES( vgl_sources
  vgl_clip                    
  vgl_distance                
  vgl_polygon                 

  vgl_polygon_test            
  vgl_lineseg_test            
  vgl_triangle_test          

  vgl_region_scan_iterator   
  vgl_polygon_scan_iterator   
  vgl_triangle_scan_iterator 
  vgl_ellipse_scan_iterator

  vgl_test                   
  vgl_window_scan_iterator   
  vgl_1d_basis               
)

AUX_SOURCE_DIRECTORY(Templates vgl_sources)

SUBDIRS( algo )
SUBDIRS( io )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
