VXL := 1
############################################
include $(IUELOCALROOT)/config/top-params.mk
############################################
USES = vxl

# Examples may use vbl....
IULIBS = -lvbl -lvnl-algo -lvnl -lnetlib -lvcl

MINI_PROG_SOURCES += vnl-calc.cxx
#MINI_PROG_SOURCES += time_fastops.cxx

###########################################
include $(IUELOCALROOT)/config/top-rules.mk
###########################################

speeds: $(RELOBJDIR)/time_fastops_O_M1

$(RELOBJDIR)/time_fastops_O_M1: time_fastops.cxx
	$(COMPILE.c++) -O2 -DOPTIMIZED -DMETHOD=1 $<
	$(MAKE_EXE) 
	$@
