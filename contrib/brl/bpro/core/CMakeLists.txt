# brl/bpro/core/CMakeLists.txt
# main package for processing operations using core classes

# vpgl process classes
add_subdirectory(vpgl_pro)

# vil process classes
add_subdirectory(vil_pro)

# vidl process classes (note it is expected that vidl will go to core)
add_subdirectory(vidl_pro)

# brip process classes
add_subdirectory(brip_pro)

# vgl and bvgl process classes
add_subdirectory(bvgl_pro)

# sdet process classes
add_subdirectory(sdet_pro)

# bbas process classes
add_subdirectory(bbas_pro)

#icam process class
add_subdirectory(icam_pro)

#ihog process class
add_subdirectory(ihog_pro)

#brad process classes
add_subdirectory(brad_pro)

