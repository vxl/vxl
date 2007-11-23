# world modeling application
# contrib/bwm/algo/CMakeLists.txt


SET(bwm_process_sources
   bwm_process.cxx                bwm_process.h
   bwm_site_process.cxx           bwm_site_process.h
   StdAfx.cpp                     StdAfx.h
)

#ADD_EXECUTABLE( bwm_process ${bwm_process_sources} )
ADD_LIBRARY(bwm_process ${bwm_process_sources})

TARGET_LINK_LIBRARIES( bwm_process bwm_algo vul)


