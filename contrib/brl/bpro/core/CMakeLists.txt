# brl/bpro/core/CMakeLists.txt
# main package for processing operations using core classes


# vpgl process classes 
SUBDIRS(vpgl_pro)

# vil process classes
SUBDIRS(vil_pro)

# vidl process classes (note it is expected that vidl will go to core)
SUBDIRS(vidl_pro)

# brip process classes 
SUBDIRS(brip_pro)

# vgl and bvgl process classes 
SUBDIRS(bvgl_pro)

# sdet process classes
SUBDIRS(sdet_pro)
