#######################################
include ${IUEROOT}/config/usr_params.mk
#######################################
USES += vxl

include $(IUEROOT)/vxl/vbl/client.mk
include $(IUEROOT)/vxl/vnl/client.mk
ald_libs := vbl vnl-algo

MINI_PROG_SOURCES += time_fastops.cxx
MINI_PROG_SOURCES += vnl_amoeba.cxx
#MINI_PROG_SOURCES += vnl-calc.cxx
MINI_PROG_SOURCES += vnl_complex_svd.cxx
MINI_PROG_SOURCES += vnl_planefit.cxx
MINI_PROG_SOURCES += vnl_polyroots.cxx
MINI_PROG_SOURCES += vnl_rosenbrock.cxx
MINI_PROG_SOURCES += vnl_svd.cxx

######################################
include ${IUEROOT}/config/usr_rules.mk
######################################
