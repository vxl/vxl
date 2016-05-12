# mul/msdi/CMakeLists.txt

doxygen_add_library(contrib/mul/msdi
  DEPENDS contrib/mul/msm contrib/mul/vimt
  PACKAGE contrib/mul
  DESCRIPTION "Manchester's Shape Data Iterator Library"
)

set(msdi_sources
  msdi_marked_images.h
  msdi_marked_images_from_files.cxx  msdi_marked_images_from_files.h
  msdi_array_of_marked_images.cxx    msdi_array_of_marked_images.h
  msdi_reflected_marked_images.cxx   msdi_reflected_marked_images.h
  msdi_points_subset.cxx             msdi_points_subset.h
)

#aux_source_directory(Templates msdi_sources)

vxl_add_library(LIBRARY_NAME msdi LIBRARY_SOURCES ${msdi_sources})
target_link_libraries(msdi msm)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
