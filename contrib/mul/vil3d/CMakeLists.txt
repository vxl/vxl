# mul/vil3d/CMakeLists.txt

SET(vil3d_sources
    vil3d_property.h
    vil3d_image_view_base.h
    vil3d_image_view.h               vil3d_image_view.txx
    vil3d_print.h
    vil3d_plane.h
    vil3d_slice.h
    vil3d_crop.h
    vil3d_transform.h
    vil3d_trilin_interp.h
    vil3d_sample_profile_trilin.h    vil3d_sample_profile_trilin.txx
    vil3d_switch_axes.h
    vil3d_math.h
    vil3d_file_format.h              vil3d_file_format.cxx
    vil3d_load.h                     vil3d_load.cxx
    vil3d_memory_image.h             vil3d_memory_image.cxx
    vil3d_copy.h                     vil3d_copy.txx
    vil3d_image_resource.h           vil3d_image_resource.cxx
    vil3d_new.h                      vil3d_new.cxx
    vil3d_convert.h

    file_formats/vil3d_gipl_format.h file_formats/vil3d_gipl_format.cxx
    file_formats/vil3d_dicom.h       file_formats/vil3d_dicom.cxx
    file_formats/vil3d_slice_list.h  file_formats/vil3d_slice_list.cxx
)
AUX_SOURCE_DIRECTORY(Templates vil3d_sources)

ADD_LIBRARY(vil3d ${vil3d_sources})

TARGET_LINK_LIBRARIES( vil3d vil vul vcl )

SUBDIRS(algo io)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
