ADD_EXECUTABLE( time_fastops time_fastops )
ADD_EXECUTABLE( vnl_amoeba vnl_amoeba )
ADD_EXECUTABLE( vnl_complex_svd vnl_complex_svd )
ADD_EXECUTABLE( vnl_planefit vnl_planefit )
ADD_EXECUTABLE( vnl_polyroots vnl_polyroots )
ADD_EXECUTABLE( vnl_rosenbrock vnl_rosenbrock )
ADD_EXECUTABLE( vnl_svd vnl_svd )
ADD_EXECUTABLE( vnl_lsqr_test vnl_lsqr_test )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/algo/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
