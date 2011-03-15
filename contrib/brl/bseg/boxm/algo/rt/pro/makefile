USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm-algo-rt-pro

SOURCES += boxm_algo_rt_register.cxx
MANPAGE_SOURCES += boxm_algo_rt_processes.h

SOURCES += processes/boxm_render_expected_edge_process.cxx
SOURCES += processes/boxm_generate_opt3_samples_process.cxx
SOURCES += processes/boxm_render_expected_depth_rt_process.cxx
SOURCES += processes/boxm_ray_statistics_process.cxx
SOURCES += processes/boxm_generate_edge_samples_process.cxx
SOURCES += processes/boxm_update_rt_process.cxx
SOURCES += processes/boxm_opt_bayesian_update_process.cxx
SOURCES += processes/boxm_render_expected_rt_process.cxx
SOURCES += processes/boxm_opt3_update_process.cxx
SOURCES += processes/boxm_edge_update_process.cxx
SOURCES += processes/boxm_generate_opt2_samples_process.cxx
SOURCES += processes/boxm_generate_opt_samples_process.cxx
SOURCES += processes/boxm_rpc_registration_process.cxx
SOURCES += processes/boxm_opt2_update_process.cxx
SOURCES += processes/boxm_change_probabilty_process.cxx
SOURCES += processes/boxm_estimate_camera_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
