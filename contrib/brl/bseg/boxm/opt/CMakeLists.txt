# brl/bseg/boxm/opt/CMakeLists.txt

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

SET(boxm_opt_sources
   boxm_opt_sample.h
   boxm_aux_traits.h
   boxm_aux_scene.txx                   boxm_aux_scene.h
   boxm_opt_rt_bayesian_optimizer.txx   boxm_opt_rt_bayesian_optimizer.h
   boxm_opt_appearance_estimator.h
)

AUX_SOURCE_DIRECTORY(Templates boxm_opt_sources)

ADD_LIBRARY(boxm_opt ${boxm_opt_sources})

TARGET_LINK_LIBRARIES(boxm_opt boxm vcl vgl)

#processes
SUBDIRS(pro)

#tests
#SUBDIRS(tests)