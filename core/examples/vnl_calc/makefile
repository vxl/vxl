
USES = vxl

VXL := 1

# Examples may use vbl....
IULIBS = -lvbl -lvnl-algo -lvnl -lnetlib -lvcl

include $(IUELOCALROOT)/config/top-params.mk

MINI_PROG_SOURCES += vnl-calc.cxx
xMINI_PROG_SOURCES += time_fastops.cxx

include $(IUELOCALROOT)/config/top-rules.mk

speeds: $(RELOBJDIR)/time_fastops_O_M1

$(RELOBJDIR)/time_fastops_O_M1: time_fastops.cxx
	$(COMPILE.c++) -O2 -DOPTIMIZED -DMETHOD=1 $<
	$(MAKE_EXE) 
	$@
