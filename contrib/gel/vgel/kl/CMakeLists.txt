#
# gel/vgel/kl/CMakeLists.txt
#
#    geoff.cross@crd.ge.com
#

ADD_LIBRARY(vgel_kl vgel_kl_sources)

SOURCE_FILES(vgel_kl_sources
convolve  
klt       
pnmio    
selectGoodFeatures  
trackFeatures
error     
klt_util  
pyramid  
storeFeatures       
writeFeatures
)

