ADD_EXECUTABLE(boxm2_vecf_fit_orbit_model orbit_fitter.cxx )	
TARGET_LINK_LIBRARIES(boxm2_vecf_fit_orbit_model boxm2_vecf vnl vgl vil vul vpl)

ADD_EXECUTABLE(boxm2_vecf_orbit_stats orbit_stats.cxx )	
TARGET_LINK_LIBRARIES(boxm2_vecf_orbit_stats boxm2_vecf vnl vgl vil vul vpl)

ADD_EXECUTABLE(boxm2_vecf_pc_orbit_viewer orbit_pc_viewer.cxx )	
TARGET_LINK_LIBRARIES(boxm2_vecf_pc_orbit_viewer boxm2_vecf vnl vgl vil vul vpl)
