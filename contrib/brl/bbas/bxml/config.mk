ifneq (,$(wildcard ${IUEROOT}/v3p/xerces-c-src1_4_0/include/dom/DOM.hpp))
 HAS_XERCES:=1
 INCDIRS += ${IUEROOT}/v3p/xerces-c-src1_4_0/include
 LIBDIRS += ${IUEROOT}/v3p/$(tj_libdir)
endif

ifdef HAS_XERCES
IULIBS += -lbxml -lxerces-c
include ${IUEROOT}/contrib/brl/bbas/bdgl/config.mk
include ${IUEROOT}/contrib/gel/vtol/config.mk
include ${IUEROOT}/contrib/gel/vdgl/config.mk
include ${IUEROOT}/contrib/gel/vsol/config.mk
include ${IUEROOT}/core/vul/config.mk
include ${IUEROOT}/core/vbl/config.mk
endif
