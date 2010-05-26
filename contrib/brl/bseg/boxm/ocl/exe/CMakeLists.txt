# this is an executable to run a process for profiling purposes
ADD_EXECUTABLE( boxm_opencl_render_expected_process
  boxm_opencl_render_expected_process_exe.cxx
 )
TARGET_LINK_LIBRARIES( boxm_opencl_render_expected_process boxm_pro  boxm_ocl boxm_ocl_pro vil_pro vpgl_pro boxm_algo_rt_pro )

