USES += vxl

ald_libs := vbl vnl-algo
IULIBS += -lvbl -lvnl-algo -lvnl -lvcl

MINI_PROG_SOURCES += time_fastops.cxx
MINI_PROG_SOURCES += vnl_amoeba.cxx
#MINI_PROG_SOURCES += vnl-calc.cxx
MINI_PROG_SOURCES += vnl_complex_svd.cxx
MINI_PROG_SOURCES += vnl_planefit.cxx
MINI_PROG_SOURCES += vnl_polyroots.cxx
MINI_PROG_SOURCES += vnl_rosenbrock.cxx
MINI_PROG_SOURCES += vnl_svd.cxx

####################################
include $(IUEROOT)/vxl/vbl/client.mk
include $(IUEROOT)/vxl/vnl/client.mk
####################################
include ${IUEROOT}/config/top.mk
################################

all:
	$(SHELL) -x ./time_fastops.mk

speeds: $(RELOBJDIR)/time_fastops_O_M1

$(RELOBJDIR)/time_fastops_O_M1: time_fastops.cxx
	$(COMPILE.c++) -O2 -DOPTIMIZED -DMETHOD=1 $<
	$(MAKE_EXE)
	$@

