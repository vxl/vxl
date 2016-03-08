# mul/vil3d/CMakeLists.txt

doxygen_add_library(contrib/mul/vil3d
  DEPENDS core/vil core/vul core/vsl
  PACKAGE contrib/mul
  DESCRIPTION "3D image library"
  )

set(vil3d_sources
    vil3d_property.h
    vil3d_image_view_base.h
    vil3d_image_view.h               vil3d_image_view.hxx
    vil3d_print.h
    vil3d_plane.h
    vil3d_slice.h
    vil3d_crop.h                     vil3d_crop.cxx
    vil3d_clamp.h
    vil3d_transform.h
    vil3d_trilin_interp.h
    vil3d_sample_profile_trilin.h    vil3d_sample_profile_trilin.hxx
    vil3d_switch_axes.h
    vil3d_math.h
    vil3d_file_format.h              vil3d_file_format.cxx
    vil3d_load.h                     vil3d_load.cxx
    vil3d_save.h                     vil3d_save.cxx
    vil3d_memory_image.h             vil3d_memory_image.cxx
    vil3d_copy.h                     vil3d_copy.cxx          vil3d_copy.hxx
    vil3d_image_resource.h
    vil3d_new.h                      vil3d_new.cxx
    vil3d_convert.h
    vil3d_fwd.h
    vil3d_from_image_2d.h
    vil3d_resample_simple.h          vil3d_resample_simple.hxx
    vil3d_resample_trilinear.h       vil3d_resample_trilinear.hxx
    vil3d_decimate.h
    vil3d_reflect.h
    vil3d_scan_image.h
    vil3d_resample_tricubic.h        vil3d_resample_tricubic.hxx
    vil3d_tricub_interp.h            vil3d_tricub_interp.hxx
    vil3d_chord.h

    file_formats/vil3d_gipl_format.h        file_formats/vil3d_gipl_format.cxx
    file_formats/vil3d_dicom.h              file_formats/vil3d_dicom.cxx
    file_formats/vil3d_slice_list.h         file_formats/vil3d_slice_list.cxx
    file_formats/vil3d_analyze_format.h     file_formats/vil3d_analyze_format.cxx
    file_formats/vil3d_gen_synthetic.h      file_formats/vil3d_gen_synthetic.cxx
    file_formats/vil3d_meta_image_format.h  file_formats/vil3d_meta_image_format.cxx
)
aux_source_directory(Templates vil3d_sources)

vxl_add_library(LIBRARY_NAME vil3d LIBRARY_SOURCES ${vil3d_sources})
target_link_libraries( vil3d ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vnl )

add_subdirectory(algo)
add_subdirectory(io)
add_subdirectory(tools)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
