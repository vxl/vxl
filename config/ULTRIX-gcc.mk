MAKE_ARLIB = $(RM) $@ && ar qcv $@ 
link-static := -Xlinker -Bstatic
link-dynamic := -Xlinker -Bdynamic
lib_dl := -ldl -rdynamic
# include generic gcc stuff that works for any system with gcc
include $(configdir)/generic-gcc.mk
ccflags += -fguiding-decls
