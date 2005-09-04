ifneq (,$(wildcard ${VXLROOT}/v3p/xerces-c-src1_4_0/include/dom/DOM.hpp))
 INCDIRS += ${VXLROOT}/v3p/xerces-c-src1_4_0/include
 include ${VXLROOT}/config/top-params.mk
 IULIBS += -lbxml $(XERCES_LIBS)
 USES += contrib/gel
 include ${VXLROOT}/contrib/brl/bbas/bdgl/config.mk
 include ${VXLROOT}/contrib/gel/vtol/config.mk
 include ${VXLROOT}/contrib/gel/vdgl/config.mk
 include ${VXLROOT}/contrib/gel/vsol/config.mk
 include ${VXLROOT}/core/vbl/config.mk
endif
