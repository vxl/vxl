#depends on OPENCL being found...
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
SET(boxm2_ocl_pro_sources
    #boxm2_opencl_process_base.h
    #boxm2_opencl_render_process.h           boxm2_opencl_render_process.cxx
    #boxm2_opencl_render_depth_process.h     boxm2_opencl_render_depth_process.cxx
    #boxm2_opencl_update_process.h           boxm2_opencl_update_process.cxx
    #boxm2_opencl_refine_process.h           boxm2_opencl_refine_process.cxx
    #boxm2_opencl_change_detection_process.h boxm2_opencl_change_detection_process.cxx
    #boxm2_opencl_update_rgb_process.h       boxm2_opencl_update_rgb_process.cxx
    #boxm2_opencl_render_rgb_process.h       boxm2_opencl_render_rgb_process.cxx
    #boxm2_opencl_batch_update_process.h     boxm2_opencl_batch_update_process.cxx
    boxm2_ocl_processes.h
    boxm2_ocl_register.h                     boxm2_ocl_register.cxx
    #boxm2_opencl_processor.h                 boxm2_opencl_processor.cxx
  )

AUX_SOURCE_DIRECTORY(Templates boxm2_ocl_pro_sources)
AUX_SOURCE_DIRECTORY(processes boxm2_ocl_pro_sources)

ADD_LIBRARY(boxm2_ocl_pro  ${boxm2_ocl_pro_sources})
TARGET_LINK_LIBRARIES(boxm2_ocl_pro boxm2_ocl_algo boct brdb vbl_io expatpp vpgl vgl vgl_xio vbl vsl vcl)

#tests
SUBDIRS(tests)

